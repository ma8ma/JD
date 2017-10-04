// ライセンス: GPL2

// 文字列関係の関数

#ifndef _MISCUTIL_H
#define _MISCUTIL_H

#include "charcode.h"

#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <glibmm.h>

namespace MISC
{
    // URLスキームタイプ
	enum
	{
		SCHEME_NONE,

		SCHEME_HTTP,
		SCHEME_FTP,
		SCHEME_TTP,
		SCHEME_TP,
                SCHEME_SSSP
	};

    // str を "\n" ごとに区切ってlistにして出力
    std::list< std::string > get_lines( const std::string& str );

    // strを空白または "" 単位で区切って list で出力
    std::list< std::string > split_line( const std::string& str );

    // strを delimで区切って list で出力
    std::list< std::string > StringTokenizer( const std::string& str, const char delim );

    // emacs lisp のリスト型を要素ごとにlistにして出力
    std::list< std::string > get_elisp_lists( const std::string& str );

    // list_inから空白行を除いてリストを返す
    std::list< std::string > remove_nullline_from_list( const std::list< std::string >& list_in );

    // list_inの各行から前後の空白を除いてリストを返す
    std::list< std::string > remove_space_from_list( const std::list< std::string >& list_in );

    // list_inからコメント行(#)を除いてリストを返す
    std::list< std::string > remove_commentline_from_list( const std::list< std::string >& list_in );

    // 空白と""で区切られた str_in の文字列をリストにして出力
    // \"は " に置換される
    // (例)  "aaa" "bbb" "\"ccc\""  → aaa と bbb と "ccc"
    std::list< std::string > strtolist( const std::string& str_in );

    // list_in の文字列リストを空白と""で区切ってストリングにして出力
    // "は \" に置換される
    // (例)  "aaa" "bbb" "\"ccc\""
    std::string listtostr( const std::list< std::string >& list_in );

    // strの前後の空白削除
    std::string remove_space( const std::string& str );

    // str前後の改行、タブ、スペースを削除
    std::string remove_spaces( const std::string& str );

    // str1からstr2で示された文字列を除く
    std::string remove_str( const std::string& str1, const std::string& str2 );

    // start 〜 end の範囲をstrから取り除く ( /* コメント */ など )
    std::string remove_str( const std::string& str, const std::string& start, const std::string& end );

    // 正規表現を使ってstr1からqueryで示された文字列を除く
    std::string remove_str_regex( const std::string& str1, const std::string& query );

    // str1, str2 に囲まれた文字列を切り出す
    std::string cut_str( const std::string& str, const std::string& str1, const std::string& str2 );

    // str1 を str2 に置き換え
    std::string replace_str( const std::string& str, const std::string& str1, const std::string& str2 );

    // str1 を str2 に置き換え( ignore case 版 )
    std::string replace_casestr( const std::string& str, const std::string& str1, const std::string& str2 );

    // list_inから str1 を str2 に置き換えてリストを返す
    std::list< std::string > replace_str_list( const std::list< std::string >& list_in,
                                               const std::string& str1, const std::string& str2 );

    // str_in に含まれる改行文字を replace に置き換え
    std::string replace_newlines_to_str( const std::string& str_in, const std::string& replace );

    // " を \" に置き換え
    inline std::string replace_quot( const std::string& str ){ return MISC::replace_str( str, "\"", "\\\"" ); }

    // \" を " に置き換え
    inline std::string recover_quot( const std::string& str ){ return MISC::replace_str( str, "\\\"", "\"" ); }

    // str 中に含まれている str2 の 数を返す
    int count_str( const std::string& str, const std::string& str2 );

    // 文字列(utf-8も) -> 整数変換
    // (例) "12３" -> 123
    // 入力: str
    // 出力:
    // dig: 桁数、0なら失敗
    // n : str から何バイト読み取ったか
    // 戻り値: 数値
    int str_to_uint( const char* str, size_t& dig, size_t& n );

    // listで指定した数字を文字に変換
    std::string intlisttostr( const std::list< int >& list_num );

    // 16進数表記文字をバイナリに変換する( 例 "E38182" -> 0xE38182 )
    // 出力 : char_out 
    // 戻り値: 変換に成功した chr_in のバイト数
    size_t chrtobin( const char* chr_in, char* chr_out );

    // strが半角でmaxsize文字を超えたらカットして後ろに...を付ける
    std::string cut_str( const std::string& str, const unsigned int maxsize );

    // 正規表現のメタ文字が含まれているか
    // escape == true ならエスケープを考慮 (例)  escape == true なら \+ → \+ のまま、falseなら \+ → \\\+
    bool has_regex_metachar( const std::string& str, const bool escape );

    // 正規表現のメタ文字をエスケープ
    // escape == true ならエスケープを考慮 (例)  escape == true なら \+ → \+ のまま、falseなら \+ → \\\+
    std::string regex_escape( const std::string& str, const bool escape );

    // 正規表現のメタ文字をアンエスケープ
    std::string regex_unescape( const std::string& str );

    // HTMLエスケープ
    // completely : URL中でもエスケープする( デフォルト = true )
    std::string html_escape( const std::string& str, const bool completely = true );

    // HTMLアンエスケープ
    std::string html_unescape( const std::string& str );

    // HTMLをプレーンテキストに変換する
    std::string to_plain( const std::string& html );

    // HTMLをPango Markupテキストに変換する
    std::string to_markup( const std::string& html );

    // URL中のスキームを判別する
    // 戻り値 : スキームタイプ
    // length    : "http://"等の文字数
    int is_url_scheme( const char* str_in, int* length = nullptr );
    int is_url_scheme_impl( const char* str_in, int* length );

    // URLとして扱う文字かどうか判別する
    // 基本 : 「!#$%&'()*+,-./0-9:;=?@A-Z_a-z~」
    // 拡張 : 「[]^|」
    //
    // "RFC 3986" : http://www.ietf.org/rfc/rfc3986.txt
    // "RFC 2396" : http://www.ietf.org/rfc/rfc2396.txt
    bool is_url_char( const char* str_in, const bool loose_url );

    // URLデコード
    std::string url_decode( const char* url, const size_t n );
    inline std::string url_decode( const std::string& url ){ return url_decode( url.c_str(), url.length() ); }

    // urlエンコード
    std::string url_encode( const char* str, const size_t n, const CharCode charcode = CHARCODE_UTF8 );
    inline std::string url_encode( const std::string& str, const CharCode charcode = CHARCODE_UTF8 )
    {
        return url_encode( str.c_str(), str.length(), charcode );
    }

    // 半角スペースまたは "" 単位で区切って url エンコード
    std::string url_encode_split( const std::string& str, const CharCode charcode );

    // BASE64
    std::string base64( const std::string& str );

    // 「&#数字;」形式の数字参照文字列の中の「数字」部分の文字列長
    //
    // in_char: 入力文字列、in_char[0] == "&" && in_char[1] == "#" であること
    // offset : 開始位置が返る
    //
    // 戻り値 : 「&#数字;」の中の数字の文字列の長さ、変換出来ないときは -1
    //
    // 例 : &#9999; なら 戻り値 = 4、 offset = 2
    //
    int spchar_number_ln( const char* in_char, int& offset );

    // 「&#数字;」形式の数字参照文字列を数字(int)に変換する
    //
    // 最初に MISC::spchar_number_ln() を呼び出して offset と lng を取得すること
    //
    // in_char: 入力文字列、in_char[0] == "&" && in_char[1] == "#" であること
    // offset : spchar_number_ln() の戻り値
    // lng : spchar_number_ln() の戻り値
    //
    // 戻り値 : 「&#数字;」の中の数字(int型)
    //
    int decode_spchar_number( const char* in_char, const int offset, const int lng );

    // str に含まれる「&#数字;」形式の数字参照文字列を全てユニーコード文字に変換する
    std::string decode_spchar_number( const std::string& str );

    // str を大文字化
    std::string toupper_str( const std::string& str );

    // list 内のアイテムを全部大文字化
    std::list< std::string > toupper_list( const std::list< std::string >& list_str );
    
    //str を小文字化
    std::string tolower_str( const std::string& str );

    // path からホスト名だけ取り出す
    // protocol = false のときはプロトコルを除く
    std::string get_hostname( const std::string& path, const bool protocol = true );

    // path からファイル名だけ取り出す
    std::string get_filename( const std::string& path );

    // path からファイル名を除いてディレクトリだけ取り出す
    std::string get_dir( const std::string& path );

    // 文字数を限定して環境変数の値を返す
    std::string getenv_limited( const char *name, const size_t size = 1 );

    // pathセパレータを / に置き換える
    std::string recover_path( const std::string& str );
    std::vector< std::string > recover_path( std::vector< std::string >&& list_str );

    // 文字列(utf-8)に全角英数字が含まれるか判定する
    bool has_widechar( const char* str );

    // 全角英数字(str1) -> 半角英数字(str2)
    // table_pos : 置き換えた文字列の位置
    void asc( const char* str1, std::string& str2, std::vector< int >& table_pos );

    // UTF-8文字列(str1) -> 正規化文字列(str2)
    // table_pos : 置き換えた文字列の位置
    void norm( const char* str1, std::string& str2, std::vector< int >* table_pos = nullptr );


    // URL中のスキームを判別する
    inline int is_url_scheme( const char* str_in, int* length )
    {
        // 候補になり得ない場合は以降の処理はしない
        if( *str_in != 'h' && *str_in != 'f' && *str_in != 't' && *str_in != 's' )
            return SCHEME_NONE;

        return is_url_scheme_impl( str_in, length );
    }
}


#endif

