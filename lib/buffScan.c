/**
 * @file buffScan.c
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-08-31
 *
 * @copyright Copyright (c) 2021
 * @addtogroup library
 * @{
 * @addtogroup buffScan
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "utilsList.h"
#include "hashMap.h"
#include "buffScan.h"
#include "internal.h"


#define TBL_SIZE 255

static bool MATCH[TBL_SIZE] = {false};

inline static void buffScan_setupTable() {
    for(int i=0;i<TBL_SIZE;i++) {
        MATCH[i] = false;
    }
}

typedef struct wordFreq_s {
    utilsList_t * commonWordList;
    float averageWordLen;
    float averageCommmonWordLen;
} wordFreq_t;

/**
 * @brief Structure to contain data for the character scanner
 *
 */
typedef bool(wordMatch)(unsigned char c);

struct fileStats_s {
    int ASCII[TBL_SIZE];
    const char * ASCII_STRINGS[TBL_SIZE];
    size_t charScanned;
    size_t wordCount;
    utilsList_t * wordList;
    hashMap_t * map;
    int whiteSpace;
    bool charFreqStats;
    bool wordsFreqStats;
    bool highestFreeWordStats;
    FILE * outfile;
    wordMatch * matchFunc;
    wordFreq_t * wordFreqStruct;
};



/**
 * @brief Cahche value for end of buffer words!
 *
 */

// struct wordState_s {
//     char * word;
// };

/**
 * @brief Callbacks for the linked list
 *
 * @param val
 */
void wordList_freeCB(void * val) {
    wordFreqAn_t * freq = (wordFreqAn_t *) val;

    free(freq->word);
    free(freq);
}

/**
 * @brief Write out the words from the word list
 * @note This currently does not take into acount that some characters,
 *       such as '<' or '>' or '&' need to be escapped in XML.
 * @param val
 */
void wordList_printCB(void * val) {
    wordFreqAn_t * freq = (wordFreqAn_t *) val;

    int newLen = 0;
    for(int i=0;i<freq->len;i++) {
        char c = (char) freq->word[i];
        switch(c) {
            case '<':
                newLen+=strlen("&lt;");
                break;
            case '>':
                newLen+=strlen("&gt;");
                break;
            case '&':
                newLen+=strlen("&amp;");
                break;
            default:
                newLen+=1;
                break;
        }
    }

    if(newLen == freq->len) {
        fprintf(freq->outfile,"\t\t<wordDef><word>%s</word><freq>%d</freq><textPercent>%0.2f%%</textPercent><len>%d</len></wordDef>\n",freq->word,freq->freq,freq->freqPercent,freq->len);
    }
    else {
        char * newWord = alloc_mem(sizeof(char)*newLen);
        for(int i=0,j=0;i<newLen;i++,j++) {
            char c = (char) freq->word[j];
            switch(c) {
                case '<':
                    newWord[i++] = '&';
                    newWord[i++] = 'l';
                    newWord[i++] = 't';
                    newWord[i] = ';';
                    // i+=3;
                    continue;
                case '>':
                    newWord[i++] = '&';
                    newWord[i++] = 'g';
                    newWord[i++] = 't';
                    newWord[i] = ';';
                    // i+=3;
                    continue;
                case '&':
                    newWord[i++] = '&';
                    newWord[i++] = 'a';
                    newWord[i++] = 'm';
                    newWord[i++] = 'p';
                    newWord[i] = ';';
                    // i+=4;
                    // newLen+=strlen("&amp;");
                    continue;
                default:
                    newWord[i] = c;
                    continue;
            }
        }
        fprintf(freq->outfile,"\t\t<wordDef><word>%s</word><freq>%d</freq><textPercent>%0.2f%%</textPercent><len>%d</len></wordDef>\n",newWord,freq->freq,freq->freqPercent,freq->len);
        free(newWord);
    }
}

/**
 * @brief Match a word stored as in a structure agains one as a char string
 *
 * @param v1 Pointer to wordFreqAn_t tructure from the list
 * @param v2 Char string to match
 * @return int
 */
inline static int wordLilst_machCB(void * v1, void * v2) {
    wordFreqAn_t * lWord = (wordFreqAn_t *) v1;
    char * word = (char*) v2;

    return strcmp(lWord->word,word);
}


/**
 * @brief Set up the ascii printable strings table
 * @todo Table needs to have better representations of character values for display,
 *       very particularly characters that would prevent XML from being parsed correctly.
 * @param tbl
 */
inline static void buffScan_setupAsciiTable(const char ** tbl) {
    tbl[0] =	"NUL";
    tbl[1] =	"SOH";
    tbl[2] =	"STX";
    tbl[3] =	"ETX";
    tbl[4] =	"EOT";
    tbl[5] =	"ENQ";
    tbl[6] =	"ACK";
    tbl[7] =	"BEL";
    tbl[8] =	"BS";
    tbl[9] =	"HT";
    tbl[10] =	"LF";
    tbl[11] =	"VT";
    tbl[12] =	"FF";
    tbl[13] =	"CR";
    tbl[14] =	"SO";
    tbl[15] =	"SI";
    tbl[16] =	"DLE";
    tbl[17] =	"DC1";
    tbl[18] =	"DC2";
    tbl[19] =	"DC3";
    tbl[20] =	"DC4";
    tbl[21] =	"NAK";
    tbl[22] =	"SYN";
    tbl[23] =	"ETB";
    tbl[24] =	"CAN";
    tbl[25] =	"EM";
    tbl[26] =	"SUB";
    tbl[27] =	"ESC";
    tbl[28] =	"FS";
    tbl[29] =	"GS";
    tbl[30] =	"RS";
    tbl[31] =	"US";
    tbl[32] =   "Space";                // space
    tbl[33] =	"Exclamation_mark";     // !
    tbl[34] =	"Double_quotes";        // "
    tbl[35] =	"Number";               // #
    tbl[36] =	"Dollar";               // $
    tbl[37] =	"Percent_sign";         // %
    tbl[38] =	"Ampersand";            // &
    tbl[39] =	"Single_quote";         // '
    tbl[40] =	"Open_parenthesis";     // (
    tbl[41] =	"Close_parenthesis";    // )
    tbl[42] =	"Asterisk";             // *
    tbl[43] =	"Plus";                 // +
    tbl[44] =	"Comma";                // ,
    tbl[45] =	"Hyphen";               // -
    tbl[46] =	"Period";               // .
    tbl[47] =	"Slash_or_divide";      // /
    tbl[48] =	"Zero";                 // 0
    tbl[49] =	"One";                  // 1
    tbl[50] =	"Two";                  // 2
    tbl[51] =	"Three";                // 3
    tbl[52] =	"Four";                 // 4
    tbl[53] =	"Five";                 // 5
    tbl[54] =	"Six";                  // 6
    tbl[55] =	"Seven";                // 7
    tbl[56] =	"Eight";                // 8
    tbl[57] =	"Nine";                 // 9
    tbl[58] =	"Colon";                // :
    tbl[59] =	"Semicolon";            // ;
    tbl[60] =	"Less_than";            // <
    tbl[61] =	"Equals";               // =
    tbl[62] =	"Greater_than";         // >
    tbl[63] =	"Question_mark";        // ?
    tbl[64] =	"At_symbol";            // @
    tbl[65] =	"Upper_case_A";
    tbl[66] =	"Upper_case_B";
    tbl[67] =	"Upper_case_C";
    tbl[68] =	"Upper_case_D";
    tbl[69] =	"Upper_case_E";
    tbl[70] =	"Upper_case_F";
    tbl[71] =	"Upper_case_G";
    tbl[72] =	"Upper_case_H";
    tbl[73] =	"Upper_case_I";
    tbl[74] =	"Upper_case_J";
    tbl[75] =	"Upper_case_K";
    tbl[76] =	"Upper_case_L";
    tbl[77] =	"Upper_case_M";
    tbl[78] =	"Upper_case_N";
    tbl[79] =	"Upper_case_O";
    tbl[80] =	"Upper_case_P";
    tbl[81] =	"Upper_case_Q";
    tbl[82] =	"Upper_case_R";
    tbl[83] =	"Upper_case_S";
    tbl[84] =	"Upper_case_T";
    tbl[85] =	"Upper_case_U";
    tbl[86] =	"Upper_case_V";
    tbl[87] =	"Upper_case_W";
    tbl[88] =	"Upper_case_X";
    tbl[89] =	"Upper_case_Y";
    tbl[90] =	"Upper_case_Z";
    tbl[91] =	"Opening_bracket";  // [
    tbl[92] =	"Backslash";        // '\'
    tbl[93] =	"Closing_bracket";  // ]
    tbl[94] =	"Caret";            // ^
    tbl[95] =	"Underscore";       // _
    tbl[96] =	"Grave_accent";     // `
    tbl[97] =	"Lower_case_a";
    tbl[98] =	"Lower_case_b";
    tbl[99] =	"Lower_case_c";
    tbl[100] =	"Lower_case_d";
    tbl[101] =	"Lower_case_e";
    tbl[102] =	"Lower_case_f";
    tbl[103] =	"Lower_case_g";
    tbl[104] =	"Lower_case_h";
    tbl[105] =	"Lower_case_i";
    tbl[106] =	"Lower_case_j";
    tbl[107] =	"Lower_case_k";
    tbl[108] =	"Lower_case_l";
    tbl[109] =	"Lower_case_m";
    tbl[110] =	"Lower_case_n";
    tbl[111] =	"Lower_case_o";
    tbl[112] =	"Lower_case_p";
    tbl[113] =	"Lower_case_q";
    tbl[114] =	"Lower_case_r";
    tbl[115] =	"Lower_case_s";
    tbl[116] =	"Lower_case_t";
    tbl[117] =	"Lower_case_u";
    tbl[118] =	"Lower_case_v";
    tbl[119] =	"Lower_case_w";
    tbl[120] =	"Lower_case_x";
    tbl[121] =	"Lower_case_y";
    tbl[122] =	"Lower_case_z";
    tbl[123] =	"Opening_brace";                        // {
    tbl[124] =	"Vertical_bar";                         // |
    tbl[125] =	"Closing_brace";                        // }
    tbl[126] =	"Equivalency_sign";                     // ~
    tbl[127] =  "DELETE";                               // &#127
    tbl[128] =	"Euro_sign";                            // €
    tbl[129] =  "10000001";                             // 10000001
    tbl[130] =  "Single_low-9_quotation_mark";          //‚
    tbl[131] =	"Latin_small_letter_f_with_hook";       //  ƒ
    tbl[132] =	"Double_low-9_quotation_mark";          // „
    tbl[133] =	"Horizontal_ellipsis";                  //  …
    tbl[134] =	"Dagger";                               // '†'
    tbl[135] =	"Double dagger";                        // ‡
    tbl[136] =	"Modifier_letter_circumflex_accent ";   // ˆ
    tbl[137] =	"Per_mille_sign";                       // ‰
    tbl[138] =	"Latin_capital_letter_S_with_caron";    // Š
    tbl[139] =	"Single_left-pointing_angle_quotation"; // ‹
    tbl[140] =	"Latin_capital_ligature_OE";            // Œ
    tbl[141] =  "10001101";                             // 10001101 unprintable character
    tbl[142] =  "Latin_capital_letter_Z_with_caron";    // Ž
    tbl[143] =  "10001111";                             // 10001111 non printable character
    tbl[144] =  "10010000";                             // 10010000 non printable character
    tbl[145] =  "Left_single_quotation_mark";           //‘
    tbl[146] =	"Right single quotation mark";          // ’
    tbl[147] =	"Left double quotation mark";                   // “
    tbl[148] =	"Right double quotation mark";                  // ”
    tbl[149] =	"Bullet";                                       // •
    tbl[150] =	"En dash";                                      // –
    tbl[151] =	"Em dash";                                      // —
    tbl[152] =	"Small tilde";                                  // ˜
    tbl[153] =	"Trade mark sign";                              // ™
    tbl[154] =	"Latin small letter S with caron";              // š
    tbl[155] =	"Single right-pointing angle quotation mark";   // ›
    tbl[156] =	"Latin_small_ligature_oe";                      // œ
    tbl[157] =  "10011101";                                     // 10011101 non printable charchter
    tbl[158] =  "Latin small letter z with caron";              // ž
    tbl[159] =	"Latin capital letter Y with diaeresis";        // Ÿ
    tbl[160] =  "10100000";                                     // 10100000 non printable character
    tbl[161] =	"Inverted_exclamation_mark";                    // ¡
    tbl[162] =	"Cent_sign";                                    // ¢
    tbl[163] =	"Pound_sign";                                   // £
    tbl[164] =	"Currency_sign";                                // ¤
    tbl[165] =	"Yen_sign";                                     // ¥
    tbl[166] =	"Pipe";                                         // ¦
    tbl[167] =	"Section_sign";                                 // §
    tbl[168] =	"umlaut";                                       // ¨
    tbl[169] =	"Copyright sign";                               // ©
    tbl[170] =	"Feminine_ordinal_indicator";                   // ª
    tbl[171] =	"Left_double_angle_quotes";                     // «
    tbl[172] =	"Not_sign";                                     // ¬
    tbl[173] =  "10101101";                                     // Soft hyphen non visible
    tbl[174] =	"Registered_trade_mark_sign";                   // ®
    tbl[175] =	"Spacing_macron";                               // ¯
    tbl[176] =	"Degree_sign";                                  // °
    tbl[177] =	"Plus-or-minus_sign";                           // ±
    tbl[178] =	"Superscript_two_squared";                      // ²
    tbl[179] =	"Superscript_three_cubed";                      // ³
    tbl[180] =	"Acute_accent";                                 // ´
    tbl[181] =	"Micro_sign";                                   // µ
    tbl[182] =	"Pilcrow_sign";                                 // ¶
    tbl[183] =	"Middle_dot";                                   // ·
    tbl[184] =	"Spacing_cedilla";                              // ¸
    tbl[185] =	"Superscript_one";                              // ¹
    tbl[186] =	"Masculine_ordinal_indicator";                  // º
    tbl[187] =	"Right_double_angle_quotes";                    // »
    tbl[188] =	"Fraction_one_quarter";                         // ¼
    tbl[189] =	"Fraction_one_half";                            // ½
    tbl[190] =	"Fraction_three_quarters";                      // ¾
    tbl[191] =	"Inverted_question_mark";                       // ¿
    tbl[192] =	"Latin_capital_letter_A_with_grave";            // À
    tbl[193] =	"Latin_capital_letter_A_with_acute";            // Á
    tbl[194] =	"Latin_capital_letter_A_with_circumflex";       // Â
    tbl[195] =	"Latin_capital_letter_A_with_tilde";            // Ã
    tbl[196] =	"Latin_capital_letter_A_with_diaeresis";        // Ä
    tbl[197] =	"Latin_capital_letter_A_with_ring_above";       // Å
    tbl[198] =	"Latin_capital_letter_AE";                      // Æ
    tbl[199] =	"Latin_capital_letter_C_with_cedilla";          // Ç
    tbl[200] =	"Latin_capital_letter_E_with_grave";            // È
    tbl[201] =	"Latin_capital_letter_E_with_acute";            // É
    tbl[202] =	"Latin_capital_letter_E_with_circumflex";       // Ê
    tbl[203] =	"Latin_capital_letter_E_with_diaeresis";        // Ë
    tbl[204] =	"Latin_capital_letter_I_with_grave";            // Ì
    tbl[205] =	"Latin_capital_letter_I_with_acute";            // Í
    tbl[206] =	"Latin_capital_letter_I_with_circumflex";       // Î
    tbl[207] =	"Latin_capital_letter_I_with_diaeresis";        // Ï
    tbl[208] =	"Latin_capital_letter_ETH";                     // Ð
    tbl[209] =	"Latin_capital_letter_N_with_tilde";            // Ñ
    tbl[210] =	"Latin_capital_letter_O_with_grave";            // Ò
    tbl[211] =	"Latin_capital_letter_O_with_acute";            // Ó
    tbl[212] =	"Latin_capital_letter_O_with_circumfle";        // Ô
    tbl[213] =	"Latin_capital_letter_O_with_tilde";            // Õ
    tbl[214] =	"Latin_capital_letter_O_with_diaeresis";        // Ö
    tbl[215] =	"Multiplication_sign";                          // ×
    tbl[216] =	"Latin capital letter O with slash";            // Ø
    tbl[217] =	"Latin_capital_letter_U_with_grave";            // Ù
    tbl[218] =	"Latin_capital_letter_U_with_acute";            // Ú
    tbl[219] =	"Latin_capital_letter_U_with_circumflex";       // Û
    tbl[220] =	"Latin_capital_letter_U_with_diaeresis";        // Ü
    tbl[221] =	"Latin_capital_letter_Y_with_acute";            // Ý
    tbl[222] =	"Latin_capital_letter_THORN";                   // Þ
    tbl[223] =	"Latin_small_letter_sharp_s";                   // ß
    tbl[224] =	"Latin_small_letter_a_with_grave";              // à
    tbl[225] =	"Latin_small_letter_a_with_acute";              // á
    tbl[226] =	"Latin_small_letter_a_with_circumflex";         // â
    tbl[227] =	"Latin_small_letter_a_with_tilde";              // ã
    tbl[228] =	"Latin_small_letter_a_with_diaeresis";          // ä
    tbl[229] =	"Latin_small_letter_a_with_ring_above";         // å
    tbl[230] =	"Latin_small_letter_ae";                        // æ
    tbl[231] =	"Latin_small_letter_c_with_cedilla";            // ç
    tbl[232] =	"Latin_small_letter_e_with_grave";              // è
    tbl[233] =	"Latin_small_letter_e_with_acute";              // é
    tbl[234] =	"Latin_small_letter_e_with_circumflex";         // ê
    tbl[235] =	"Latin_small_letter_e_with_diaeresis";          // ë
    tbl[236] =	"Latin_small_letter_i_with_grave";              // ì
    tbl[237] =	"Latin_small_letter_i_with_acute";              // í
    tbl[238] =	"Latin_small_letter_i_with_circumflex";         // î
    tbl[239] =	"Latin_small_letter_i_with_diaeresis";          // ï
    tbl[240] =	"Latin-small_letter_eth";                       // ð
    tbl[241] =	"Latin_small_letter_n_with_tilde";              // ñ
    tbl[242] =	"Latin_small_letter_o_with_grave";              // ò
    tbl[243] =	"Latin_small_letter_o_with_acute";              // ó
    tbl[244] =	"Latin_small_letter_o_with_circumflex";         // ô
    tbl[245] =	"Latin_small_letter_o_with_tilde";              // õ
    tbl[246] =	"Latin_small_letter_o_with_diaeresis";          // ö
    tbl[247] =	"Division_sign";                                // ÷
    tbl[248] =	"Latin_small_letter_o_with_slash";              // ø
    tbl[249] =	"Latin_small_letter_u_with_grave";              // ù
    tbl[250] =	"Latin_small_letter_u_with_acute";              // ú
    tbl[251] =	"Latin_small_letter_u_with_circumflex";         // û
    tbl[252] =	"Latin_small_letter_u_with_diaeresis";          // ü
    tbl[253] =	"Latin_small_letter_y_with_acute";              // ý
    tbl[254] =	"Latin_small_letter_thorn";                     // þ
    tbl[255] =	"Latin_small_letter_y_with_diaeresis";          // ÿ
}

void buffScan_addCharToMatchFunc(unsigned char c) {
    MATCH[c] = true;
}

inline static uint32_t generate_key(void* data) {
    return hashMap_hashString32((const char *)data);
}

inline static void print_string(void * data) {
    printf("\"%s\"", (const char *)data);
}

inline static bool match_string(void * v1, void* v2) {
    return wordLilst_machCB(v1,v2) == 0;
}

/**
 * @brief Match characters in a text file table
 *
 * @param c
 * @return true
 * @return false
 */
bool isTableMatch(unsigned char c) {
    return MATCH[c];
}

/**
 * @brief Match [A-Za-z]
 *
 * @param c
 * @return true
 * @return false
 */
bool isWordChar(unsigned char c) {
    switch((char)c) {
        case 'a' ... 'z':     return true;
        case 'A' ... 'Z':    return true;
        default:
            break;
    }

    return false;
}

/**
 * @brief Match any valid character from [!-~]
 *
 * @param c
 * @return true
 * @return false
 */
inline static bool isValidChar(unsigned char c) {
    switch(c) {
        case 33 ... 126:    return true;
        default:
            break;
    }

    return false;
}

/**
 * @brief Match any printable character..
 *
 * @param c
 * @return true
 * @return false
 */
inline static bool isPrintableCharacter(unsigned char c) {
    switch(c) {
        case 33 ... 126:    return true;
        case 130 ... 140:   return true;
        case 145 ... 156:   return true;
        case 158 ... 255:   return true;
        default:
            break;
    }

    return false;
}


/**
 * @brief Create a file scanner with default settings
 *
 * @return fileStats_t*
 */
fileStats_t * buffScan_init(size_t mapSize) {
    fileStats_t * buff = (fileStats_t *) alloc_mem(sizeof(fileStats_t));
    if(buff) {
        listInit_t cbs = {"wordList",wordList_freeCB,wordList_printCB,wordLilst_machCB};

        for(int i=0;i<TBL_SIZE;i++) {
            buff->ASCII[i] = 0;
        }

        // Setup tables
        buffScan_setupAsciiTable(buff->ASCII_STRINGS);
        buffScan_setupTable();

        // Word list for itteration. Might need to do away with this!
        buff->wordList = utilsListMake(&cbs);
        if(buff->wordList == NULL) {
            free(buff);
            return NULL;
        }

        // Create the map with no delete callback so we don't have to worry about memory stored in the list
        hashMapInit_t mapInit = {"wordMap", generate_key,match_string,NULL,print_string,mapSize};
        buff->map = hashMap_make(&mapInit);

        buff->whiteSpace = 0;
        buff->charScanned = 0;
        buff->charFreqStats   = false;
        buff->wordsFreqStats  = false;
        buff->highestFreeWordStats = false;
        buff->wordCount = 0;

        // Default to standard out.
        buff->outfile  = stdout;

        buff->matchFunc = isWordChar;

        // This will get made when we need it...
        buff->wordFreqStruct = NULL;
    }

    return buff;
}

void buffScan_setWordStats(fileStats_t * stats, bool val) {
    if(stats) {
        stats->wordsFreqStats = val;
    }
}

void buffScan_setCharFreqStats(fileStats_t * stats, bool val) {
    if(stats) {
        stats->charFreqStats = val;
    }
}

void buffScan_setHighestFreqWordsStats(fileStats_t * stats, bool val) {
    if(stats) {
        stats->highestFreeWordStats = val;
    }

}

void buffScan_setOutputFile(fileStats_t * stats, FILE * file) {
    if(stats) {
        stats->outfile = file;
    }
}

void buffScan_setWordMatch(fileStats_t * stats, const char * funcName) {
    if(stats){
        if(strcmp(funcName,"isValidChar") == 0) {
            stats->matchFunc = isValidChar;
        }
        else if(strcmp(funcName,"isWordChar") == 0) {
            stats->matchFunc = isWordChar;
        }
        else if(strcmp(funcName,"isPrintableCharacter") == 0) {
            stats->matchFunc = isPrintableCharacter;
        }
        else if(strcmp(funcName,"isTableMatch") == 0) {
            stats->matchFunc = isTableMatch;
        }
    }
}


inline static bool isWhiteSpace(unsigned char c) {
    switch(c) {
        case 0 ... 32:      return true;
        case 127:           return true;
        case 129:           return true;
        case 141:           return true;
        case 143 ... 144:   return true;
        case 157:           return true;
        case 160:           return true;
        // case 173:           return true;
        default:
            break;
    }

    return false;
}


/**
 * @brief Identify an in word charcter, that might appear in a word
 *        that is not alpha such as @ or &
 *
 * @param c
 * @return true
 * @return false
 */
inline static bool isInWordChar(unsigned char c) {
    if(isWordChar(c)) {return true;}

    switch(c) {
        case 48 ... 57: return true; // Numeric characters, i.e. B2B
        case 38:        return true; // & i.e. B&B
        case 64:        return true; // @ i.e. barry.w.robinson64@gmail.com
        case 46:        return true; // . as above. Need to be carfull here, also comes at the end of a sentance!
        default:
            break;
    }

    return false;
}

void doWordFound(fileStats_t* stats,const unsigned char * buffer, size_t start, size_t buffLen) {
    if(buffLen > 1) {

        char * wordBuffer = (char *) alloc_mem(sizeof(char) * buffLen+1);
        strncpy(wordBuffer,(char*) &buffer[start],buffLen);

        wordBuffer[buffLen] = '\0';

        // printf("WORD: %s\n", wordBuffer);
        wordFreqAn_t * freq = (wordFreqAn_t *) hashMap_getWithKey(stats->map,(void*) wordBuffer);

        if(freq) {
            free(wordBuffer);
            freq->freq++;
        }
        else if(buffLen > 1) {
            freq = alloc_mem(sizeof(wordFreqAn_t));
            freq->word = wordBuffer;
            freq->freq = 1;
            freq->len = strlen(wordBuffer);
            freq->outfile = stats->outfile;

            // Add to storage for later processing
            utilsListAddHead(stats->wordList,(void*)freq);
            hashMap_putDataWithKey(stats->map,freq,wordBuffer);
        }

        stats->wordCount++;
    }
}

/**
 * @brief Scanner to identify the freequenct of words and letters in a text file.
 * @note This construction of the scanner means that it would be difficult to identify
 * complex words consisting of non alpha characters, such as &#91;, which is the HTLM
 * character for '['. If this where required a btter approache would be to define a parse,
 * (possibly using Flex and Bison) and parse the file for specific character sequences.
 * @param stats
 * @param buffer
 * @param buffLen
 * @return int
 */
size_t buffScan_scan(fileStats_t* stats,const unsigned char * buffer, size_t buffLen) {
    if(buffer == NULL) {
        fprintf(stderr,"ERROR: buffer can't be null\n");
        return -1;
    }

    if(buffLen <= 0) {
        fprintf(stderr,"ERROR: buffer length must be a possitive no negative number\n");
        return -1;
    }

    // If word freequency stats have been claculated, assume we are doing a second parse
    // and reset the stats. Words in the list will be untouched!
    if(stats->wordsFreqStats && stats->wordFreqStruct) {
        fprintf(stderr,"WARNING: Wordstats structure is still in use. Removing\n");
        utilsListDelete(stats->wordFreqStruct->commonWordList);
        free(stats->wordFreqStruct);
        stats->wordFreqStruct = NULL;
    }


    // Now scan the buffer for words. Whitespaces have already been scaned and counted.
    bool word=false;
    size_t startIdx=0;

    size_t i=0;
    for(/*declared above*/i=0;i<buffLen;i++) {
        if(word && stats->matchFunc(buffer[i])) {
            continue;
        }
        else if(!word && stats->matchFunc(buffer[i])) {
            word=true;
            startIdx=i;
        }
        else if(!stats->matchFunc(buffer[i])) {
            if(isWhiteSpace(buffer[i])) {
                stats->whiteSpace++;
            }

            if(word) {
                word=false;
                doWordFound(stats,buffer, startIdx, i-startIdx);
            }
        }
    }

    if(stats->charFreqStats) {
        for(i=0;i<buffLen;i++) {
            char c = buffer[i];
            stats->ASCII[(int)c]++;
            stats->charScanned++;
        }
    }

    return i; // Number of characters scanned
}

/**
 * @brief Release the scanner and al it's memory
 *
 * @param stats to be released
 */
void buffScan_release(fileStats_t* stats) {
    // Release the list
    utilsListDelete(stats->wordList);
    stats->wordList = NULL;

    if(stats->wordFreqStruct) {
        utilsListDelete(stats->wordFreqStruct->commonWordList);
        free(stats->wordFreqStruct);
        stats->wordFreqStruct = NULL;
    }


    // Release the map
    hashMap_teardown(stats->map);
    stats->map = NULL;

    free(stats);
}

/**
 * @brief Print word stats to the designated output stream
 *
 * @param stats
 */
void buffScan_printWords(fileStats_t* stats) {
    if(stats != NULL) {
        if(stats->wordsFreqStats) {
            utilsListPrint(stats->wordList);
        }
    }
}

/**
 * @brief Get the Word List object, a list of scanned words.
 *
 * @param stats
 * @return utilsList_t*
 */
utilsList_t * getWordList(fileStats_t* stats) {
    if(stats) {
        return stats->wordList;
    }

    return NULL;
}

/**
 * @brief Get the White Space Count
 *
 * @param stats
 * @return int
 */
int getWhiteSpaceCount(fileStats_t* stats) {
    if(stats) {
        return stats->whiteSpace;
    }

    return -1;
}

/**
 * @brief Simple helper functions for output
 *
 * @param stats
 */
inline static void printTableStart(fileStats_t* stats) {
    fprintf(stats->outfile, "<table>\n");

}

inline static void printTableEnd(fileStats_t* stats) {
    fprintf(stats->outfile, "</table>\n");
}

inline static void printStartCharStats(fileStats_t* stats) {
    fprintf(stats->outfile, "\t<charFreq>\n");
}

inline static void printEndCharStats(fileStats_t* stats) {
    fprintf(stats->outfile,"\t</charFreq>\n");
}

/**
 * @brief Output the word analysis table
 *
 * @param stats
 */
void printWordAnalasysTable(fileStats_t* stats) {
    fprintf(stats->outfile,"\t<wordFreq>\n");

    utilsList_t * list = getWordList(stats);
    utilsListPrint(list);
    fprintf(stats->outfile,"\t</wordFreq>\n");
}

inline static void buildWordFrequencyStats(fileStats_t* stats) {
    wordFreq_t * freqStats = NULL;
    if(stats) {
        if(stats->wordFreqStruct == NULL) {
            freqStats = alloc_mem(sizeof(wordFreq_t));
            if(freqStats == NULL) {
                return;
            }

            // We don't want to free on list deletion, so don't add callback
            listInit_t init = {"highFreqWords", NULL, wordList_printCB,wordLilst_machCB};
            freqStats->commonWordList = utilsListMake(&init);
            if(freqStats->commonWordList == NULL) {
                printf("Failed to make common word list\n");
                return;
            }

            stats->wordFreqStruct = freqStats;
        }

        utilsListItem_t * it = utilsListGetTail(stats->wordList);
        if(it == NULL) {return;} // There are no words!

        // Iterate through the list to find the word with the highest freequency
        int high=0;
        size_t totalWordLen=0;
        size_t averageCommmonWordLen=0;
        wordFreqAn_t * highFreq = NULL;
        while(it) {

            // Find the highest freequency word
            wordFreqAn_t * freq = (wordFreqAn_t*) utilsListGetData(it);
            if(freq->freq > high) {
                highFreq = freq;
                high=freq->freq;
            }

            // This will be used to calculate the average word length
            totalWordLen+=freq->len;
            it = utilsListGetNext(it);
        }

        // Add our high freequency to the list
        utilsListAddHead(freqStats->commonWordList, highFreq);
        averageCommmonWordLen+=highFreq->len;

        // Now do it again to find other words that have the same freequency
        it = utilsListGetTail(stats->wordList);
        while(it) {
            wordFreqAn_t * freq = (wordFreqAn_t*) utilsListGetData(it);
            // Update the percentage freequancy of each word
            freq->freqPercent = (double) ((double) freq->freq / (double) stats->wordCount) * 100;
            if(freq->freq == highFreq->freq && freq != highFreq) {
                averageCommmonWordLen+=freq->len;
                utilsListAddHead(freqStats->commonWordList, freq);
            }
            it = utilsListGetNext(it);


        }

        freqStats->averageCommmonWordLen = (float) ((float) averageCommmonWordLen / (float) utilsListGetSize(freqStats->commonWordList));
        freqStats->averageWordLen = (float) ((float) totalWordLen / (float) utilsListGetSize(stats->wordList));
    }
}



inline static void printHighestFreqWords(fileStats_t* stats) {
    if(utilsListGetSize(stats->wordFreqStruct->commonWordList) > 0){
        fprintf(stats->outfile, "\t<highestFreqWords>\n");
        utilsListPrint(stats->wordFreqStruct->commonWordList);
        fprintf(stats->outfile, "\t</highestFreqWords>\n");
    }
}
/**
 * @brief Output all freequency analysis statistics and tables
 *
 * @param stats
 */
void printFrequencyAnalysis(fileStats_t* stats) {
    buildWordFrequencyStats(stats);

    printTableStart(stats);

    if(stats->charFreqStats) {
        printStartCharStats(stats);
        for(int i=0;i<TBL_SIZE;i++) {
            if(stats->ASCII[i] > 0) {
                double precent = (double)((double)stats->ASCII[i] / (double)stats->charScanned) * 100;
                fprintf(stats->outfile,"\t\t<%s>%d</%s><charPersent>%0.2f<charPersent>\n", stats->ASCII_STRINGS[i],stats->ASCII[i],stats->ASCII_STRINGS[i],precent);
            }
        }
        printEndCharStats(stats);
    }

    if(stats->wordsFreqStats) {
        printWordAnalasysTable(stats);
    }

    if(stats->highestFreeWordStats) {
        printHighestFreqWords(stats);
    }

    // This is always output.
    float percent = 0.0;
    if(stats->whiteSpace == stats->charScanned) {
        percent = 100;
    }
    else {
        percent = ((float) stats->whiteSpace / (float) stats->charScanned) * 100;
    }

    fprintf(stats->outfile,"\t<whiteSpaceCount>%d</whiteSpaceCount><percent>%0.2f%%</percent>\n",  stats->whiteSpace,percent);
    fprintf(stats->outfile, "\t<averageWordLen>%0.2f</averageWordLen>\n",                          stats->wordFreqStruct->averageWordLen);
    fprintf(stats->outfile, "\t<averageHighFreqWorLen>%0.2f</averageHighFreqWorLen>\n",            stats->wordFreqStruct->averageCommmonWordLen);
    fprintf(stats->outfile,"\t<numCharScanned>%ld</numCharScanned>\n",                             stats->charScanned);
    printTableEnd(stats);
}

hashMap_t * getHashMap(fileStats_t* stats) {
    if(stats) {
        return stats->map;
    }

    return NULL;
}

/**
 * @}
 * @}
 */
