#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal) {
    size_t i=0;
    EXPECT(c, *literal);
    while (literal[i+1]!='\0') {
        if (c->json[i]!=literal[i+1]) return LEPT_PARSE_INVALID_VALUE;
        i++;
    }
    c->json +=i;
    switch (*literal) {
        case 't':  v->type = LEPT_TRUE;break;
        case 'f':  v->type = LEPT_FALSE;break;
        case 'n':  v->type = LEPT_NULL;break;
    }
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    const char* temp = c->json;
    if (temp[0]=='-') temp++;
    if (!ISDIGIT(temp[0])) return LEPT_PARSE_INVALID_VALUE;
    while (*temp!='\0') temp++;
    if (*(temp-1)=='.') return LEPT_PARSE_INVALID_VALUE;
//    if (temp[0]=='0') {
//        if (temp[1]!='\0'&&temp[1]!='.') return LEPT_PARSE_INVALID_VALUE;
//        if (temp[1]=='.') {
//            if (!ISDIGIT(temp[2])) return LEPT_PARSE_INVALID_VALUE;
//        }
//    } else {
//    if (temp[0]!='0') {
//        while (ISDIGIT(*temp)) temp++;
//        if (*temp=='.') {
//            temp++;
//            if (*temp=='\0') return LEPT_PARSE_INVALID_VALUE;
//            while (ISDIGIT(*temp)) temp++;
//            if (*temp == 'E'||*temp == 'e') {
//                *temp++;
//                if (*temp=='-'||*temp =='+') temp++;
//                while (ISDIGIT(*temp)) temp++;
//                if (*temp!='\0') return LEPT_PARSE_INVALID_VALUE;
//            } else if (*temp!='\0') return LEPT_PARSE_INVALID_VALUE;
//        } else if (*temp == 'E'||*temp=='e') {
//            *temp++;
//            if (*temp=='-'||*temp =='+') temp++;
//            while (ISDIGIT(*temp)) temp++;
//            if (*temp!='\0') return LEPT_PARSE_INVALID_VALUE;
//        } else {
//            if (*temp != '\0') return  LEPT_PARSE_INVALID_VALUE;
//        }
//    }
    /*
     * strtod 功能：将一个字符串转化称double，并将剩余的字符串返回
     * 参数：需要转化的字符串，指向剩余字符串的字符串地址
     * 返回：解析出的第一个double数字
     */
    v->n = strtod(c->json, &end);

    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v, "true");
        case 'f':  return lept_parse_literal(c, v, "false");
        case 'n':  return lept_parse_literal(c, v, "null");
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
