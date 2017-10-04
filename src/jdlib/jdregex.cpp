// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "jdregex.h"
#include "miscutil.h"

#ifdef HAVE_MIGEMO_H
#include "jdmigemo.h"
#endif

#include <cstring>

constexpr std::size_t MAX_TARGET_SIZE = 64 * 1024;  // 全角半角変換のバッファサイズ
#if USE_REGEX_COMPAT
constexpr std::size_t REGEX_MAX_NMATCH = 32;
#endif


using namespace JDLIB;


RegexPattern::RegexPattern( const std::string& reg, const bool icase, const bool newline,
                            const bool usemigemo, const bool wchar, const bool norm )
{
    set( reg, icase, newline, usemigemo, wchar, norm );
}


RegexPattern::~RegexPattern()
{
    clear();
}

void RegexPattern::clear()
{
    if( m_compiled )
#if USE_REGEX_COMPAT
        regfree( &m_regex );
#else
        g_regex_unref( m_regex );
#endif
    m_compiled = false;

#if USE_REGEX_COMPAT
    m_error = 0;
#else
    g_clear_error( &m_error );
#endif
}

// icase : 大文字小文字区別しない
// newline :  . に改行をマッチさせない
// usemigemo : migemo使用 (コンパイルオプションで指定する必要あり)
// wchar : 全角半角の区別をしない
bool RegexPattern::set( const std::string& reg, const bool icase, const bool newline,
                        const bool usemigemo, const bool wchar, const bool norm )
{
#ifdef _DEBUG
    if( wchar ){
        std::cout << "RegexPattern::set " << reg << std::endl;
    }
#endif

    if( m_compiled ){
#if USE_REGEX_COMPAT
        regfree( &m_regex );
#else
        g_regex_unref( m_regex );
#endif
        m_compiled = false;
    }
    
#if USE_REGEX_COMPAT
    m_error = 0;
#else
    g_clear_error( &m_error );
#endif
    if( reg.empty() ) return false;
    
#if USE_REGEX_COMPAT
#ifdef HAVE_PCREPOSIX_H
    int cflags = REG_UTF8;
    if( ! newline ) cflags |= REG_DOTALL; // . を改行にマッチさせる
#else
    int cflags = REG_EXTENDED;
#endif
    if( newline ) cflags |= REG_NEWLINE;
    if( icase ) cflags |= REG_ICASE;
#else
    int cflags = G_REGEX_OPTIMIZE;
    if( newline ) cflags |= G_REGEX_MULTILINE;
    else cflags |= G_REGEX_DOTALL; // . を改行にマッチさせる
    if( icase ) cflags |= G_REGEX_CASELESS;
#endif //USE_REGEX_COMPAT

    m_newline = newline;
    m_wchar = wchar;
    m_norm = norm;

    const char* asc_reg = reg.c_str();
    std::string target_asc;

    // Unicode正規化
    if( m_norm ){
        target_asc.reserve( MAX_TARGET_SIZE );
        MISC::norm( asc_reg, target_asc );
        asc_reg = target_asc.c_str();

#ifdef _DEBUG
        std::cout << target_asc << std::endl;
#endif
    }

    // 全角英数字 → 半角英数字、半角カナ → 全角カナ
    else if( m_wchar && MISC::has_widechar( asc_reg ) ){

        target_asc.reserve( MAX_TARGET_SIZE );
        std::vector<int> temp;
        MISC::asc( asc_reg, target_asc, temp );
        asc_reg = target_asc.c_str();

#ifdef _DEBUG
        std::cout << target_asc << std::endl;
#endif
    }

#ifdef HAVE_MIGEMO_H

    if( usemigemo ){
        const std::string mgm_reg = jd_migemo_regcreate( asc_reg );
        if( !mgm_reg.empty() ){
#if USE_REGEX_COMPAT
            m_error = regcomp( &m_regex, mgm_reg.c_str(), cflags );
            if( m_error != 0 ) regfree( &m_regex );
#else
            m_regex = g_regex_new( mgm_reg.c_str(), GRegexCompileFlags( cflags ), GRegexMatchFlags( 0 ), &m_error );
            if( m_regex == NULL ) g_clear_error( &m_error );
#endif
            else goto compile_done;
        }
    }
#endif //HAVE_MIGEMO_H

#if USE_REGEX_COMPAT
    m_error = regcomp( &m_regex, asc_reg, cflags );
    if( m_error != 0 ){
        regfree( &m_regex );
        return false;
    }
#else
    m_regex = g_regex_new( asc_reg, GRegexCompileFlags( cflags ), GRegexMatchFlags( 0 ), &m_error );
    if( m_regex == NULL ){
        return false;
    }
#endif

#ifdef HAVE_MIGEMO_H
compile_done:
#endif

    m_compiled = true;
    return true;
}


std::string RegexPattern::errstr() const
{
    std::string errmsg;

#if USE_REGEX_COMPAT
    switch( m_error ){
        case 0: errmsg = "エラーはありません。"; break;
        case REG_BADBR: errmsg = "無効な後方参照オペレータを使用しています。"; break;
        case REG_BADPAT: errmsg = "無効なグループやリストなどを使用しています。"; break;
        case REG_BADRPT: errmsg = "'*' が最初の文字としてくるような、無効な繰り返しオペレータを使用しています。"; break;
        case REG_EBRACE: errmsg = "インターバルオペレータ {} が閉じていません。"; break;
        case REG_EBRACK: errmsg = "リストオペレータ [] が閉じていません。"; break;
        case REG_ECOLLATE: errmsg = "照合順序の要素として有効ではありません。"; break;
        case REG_ECTYPE: errmsg = "未知のキャラクタークラス名です。"; break;
#ifdef HAVE_REGEX_H
        case REG_EEND: errmsg = "未定義エラー。POSIX.2 には定義されていません。"; break;
#endif
        case REG_EESCAPE: errmsg = "正規表現がバックスラッシュで終っています。"; break;
        case REG_EPAREN: errmsg = "グループオペレータ () が閉じていません。"; break;
        case REG_ERANGE: errmsg = "無効な範囲オペレータを使用しています。"; break;
#ifndef HAVE_ONIGPOSIX_H
        case REG_ESIZE: errmsg = "複雑過ぎる正規表現です。POSIX.2 には定義されていません。"; break;
#endif
        case REG_ESPACE: errmsg = "regex ルーチンがメモリーを使い果たしました。"; break;
        case REG_ESUBREG: errmsg = "サブエクスプレッション (...) への無効な後方参照です。"; break;
        default: errmsg = "未知のエラーが発生しました。"; break;
    }
#else
    errmsg = m_error->message;
#endif
    return errmsg;
}


///////////////////////////////////////////////

Regex::Regex() noexcept = default;


Regex::~Regex() noexcept = default;


bool Regex::match( const RegexPattern& creg, const std::string& target, const size_t offset, const bool notbol, const bool noteol )
{
    m_pos.clear();
    m_results.clear();

    if ( ! creg.m_compiled ) return false;

    if( target.empty() ) return false;
    if( target.length() <= offset ) return false;

    const char* asc_target = target.c_str() + offset;

    bool exec_asc = false;

    // Unicode正規化
    if( creg.m_norm ){

#ifdef _DEBUG
        std::cout << "Regex::match offset = " << offset << std::endl;
        std::cout << target << std::endl;
#endif

        m_target_asc.clear();
        m_table_pos.clear();
        if( m_target_asc.capacity() < MAX_TARGET_SIZE ) {
            m_target_asc.reserve( MAX_TARGET_SIZE );
            m_table_pos.reserve( MAX_TARGET_SIZE );
        }

        MISC::norm( asc_target, m_target_asc, &m_table_pos );
        exec_asc = true;
        asc_target = m_target_asc.c_str();

#ifdef _DEBUG
        std::cout << m_target_asc << std::endl;
#endif
    }

    // 全角英数字 → 半角英数字、半角カナ → 全角カナ
    else if( creg.m_wchar && MISC::has_widechar( asc_target ) ){

#ifdef _DEBUG
        std::cout << "Regex::match offset = " << offset << std::endl;
        std::cout << target << std::endl;
#endif

        m_target_asc.clear();
        m_table_pos.clear();
        if( m_target_asc.capacity() < MAX_TARGET_SIZE ) {
            m_target_asc.reserve( MAX_TARGET_SIZE );
            m_table_pos.reserve( MAX_TARGET_SIZE );
        }

        MISC::asc( asc_target, m_target_asc, m_table_pos );
        exec_asc = true;
        asc_target = m_target_asc.c_str();

#ifdef _DEBUG
        std::cout << m_target_asc << std::endl;
#endif
    }

#if USE_REGEX_COMPAT
    regmatch_t pmatch[ REGEX_MAX_NMATCH ];
    memset( pmatch, 0, sizeof( pmatch ));

    int eflags = 0;
    if( notbol ) eflags |= REG_NOTBOL;
    if( noteol ) eflags |= REG_NOTEOL;
#else
    GMatchInfo *pmatch;

    int eflags = 0;
    if( notbol ) eflags |= G_REGEX_MATCH_NOTBOL;
    if( noteol ) eflags |= G_REGEX_MATCH_NOTEOL;
#endif

#ifdef HAVE_ONIGPOSIX_H

    // 鬼車はnewlineを無視するようなので、文字列のコピーを取って
    // 改行をスペースにしてから実行する
    if( ! creg.m_newline ){

        std::string target_copy = asc_target;
        std::replace( target_copy.begin(), target_copy.end(), '\n', ' ' );
        if( regexec( &creg.m_regex, target_copy.c_str(), REGEX_MAX_NMATCH, pmatch, eflags ) != 0 ){
            return false;
        }
    }
    else

#endif

#if USE_REGEX_COMPAT
    if( regexec( &creg.m_regex, asc_target, REGEX_MAX_NMATCH, pmatch, eflags ) != 0 ){
        return false;
    }
    const int match_count = REGEX_MAX_NMATCH;
#else
    if( ! g_regex_match( creg.m_regex, asc_target, GRegexMatchFlags( eflags ), &pmatch ) ){
        g_match_info_free( pmatch );
        return false;
    }
    const int match_count = g_match_info_get_match_count( pmatch ) + 1;
#endif

    for( int i = 0; i < match_count; ++i ){

        int so, eo;
#if USE_REGEX_COMPAT
        so = pmatch[ i ].rm_so;
        eo = pmatch[ i ].rm_eo;
#else
        if( ! g_match_info_fetch_pos( pmatch, i, &so, &eo ) ) so = eo = -1;
#endif

        if( so < 0 || eo < 0 ){
            m_pos.push_back( so );
            m_results.push_back( std::string() );
        }

        else{
            if( exec_asc ){
#ifdef _DEBUG
                std::cout << "so=" << so << " eo=" << eo;
#endif
                while( so > 0 && m_table_pos[ so ] < 0 ) so--;
                so = m_table_pos[ so ];
                while( m_table_pos[ eo ] < 0 ) eo++;
                eo = m_table_pos[ eo ];
#ifdef _DEBUG
                std::cout << " -> so=" << so << " eo=" << eo << std::endl;
#endif
            }
            so += offset;
            eo += offset;

            m_pos.push_back( so );
            m_results.push_back( target.substr( so, eo - so ) );
        }
    }
    
#if !USE_REGEX_COMPAT
    g_match_info_free( pmatch );
#endif

    return true;
}


//
// マッチした文字列と $0〜$9 or \0〜\9 を置換する
//
std::string Regex::replace( const std::string& repstr ) const
{
    if( repstr.empty() ) return repstr;

    const char* p0 = repstr.c_str();
    const char* p1;
    std::string str_out;

    while( ( p1 = strchr( p0, '\\' ) ) != nullptr ){
        int n = p1[ 1 ] - '0';
        str_out.append( p0, p1 - p0 );
        p0 = p1 + 2;
        if( n < 0 || n > 9 ){
            str_out.push_back( p1[ 1 ] );
        }
        else if( m_results.size() > static_cast< size_t >( n ) && m_pos[ n ] != -1 ){
            str_out += m_results[ n ];
        }
    }
    
    str_out.append( repstr, p0 - repstr.c_str(), std::string::npos );
    
    return str_out;
}


std::string Regex::str( const size_t num ) const
{
    if( m_results.size() > num  ) return m_results[ num ];

    return std::string();
}


int Regex::pos( const size_t num ) const noexcept
{
    if( m_results.size() > num ) return m_pos[ num ];

    return -1;
}
