// Copyright �2018 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in innative.h

#ifndef __WAT_H__IR__
#define __WAT_H__IR__

#include "innative/schema.h"
#include "util.h"
#include "queue.h"
#include "stack.h"
#include <istream>

namespace innative {
  namespace wat {
    typedef unsigned short WatTokenID;

    enum WatTokens : WatTokenID
    {
      TOKEN_NONE = 0, // Error state
      TOKEN_OPEN, // (
      TOKEN_CLOSE, // )
      TOKEN_MODULE,
      TOKEN_IMPORT,
      TOKEN_TYPE,
      TOKEN_START,
      TOKEN_FUNC,
      TOKEN_GLOBAL,
      TOKEN_TABLE,
      TOKEN_MEMORY,
      TOKEN_EXPORT,
      TOKEN_DATA,
      TOKEN_ELEM,
      TOKEN_OFFSET,
      TOKEN_ALIGN,
      TOKEN_LOCAL,
      TOKEN_RESULT,
      TOKEN_PARAM,
      TOKEN_i32,
      TOKEN_i64,
      TOKEN_f32,
      TOKEN_f64,
      TOKEN_ANYFUNC,
      TOKEN_MUT,
      TOKEN_BLOCK,
      TOKEN_LOOP,
      TOKEN_IF,
      TOKEN_THEN,
      TOKEN_ELSE,
      TOKEN_END,
      TOKEN_BINARY, // Script extension tokens
      TOKEN_QUOTE,
      TOKEN_REGISTER,
      TOKEN_INVOKE,
      TOKEN_GET,
      TOKEN_ASSERT_RETURN,
      TOKEN_ASSERT_RETURN_CANONICAL_NAN,
      TOKEN_ASSERT_RETURN_ARITHMETIC_NAN,
      TOKEN_ASSERT_TRAP,
      TOKEN_ASSERT_MALFORMED,
      TOKEN_ASSERT_INVALID,
      TOKEN_ASSERT_UNLINKABLE,
      TOKEN_ASSERT_EXHAUSTION,
      TOKEN_SCRIPT,
      TOKEN_INPUT,
      TOKEN_OUTPUT,
      TOKEN_NUMBER,
      TOKEN_STRING,
      TOKEN_NAME, // Same as string but more restricted
      TOKEN_OPERATOR, // Represents all operators other than the control flow ones above
      TOKEN_COMMENT,
      TOKEN_RANGE_ERROR,
      TOKEN_INTEGER, // Explicit integer/float are only used as a shortcut when generating token lists
      TOKEN_FLOAT,
      TOKEN_TOTALCOUNT,
    };

    typedef struct __WAT_TOKEN
    {
      WatTokenID id;
      const char* pos;
      size_t line;
      size_t column;
      union
      {
        int64_t i;
        double f;
        size_t len;
      };
    } WatToken;

    struct DeferWatAction
    {
      int id;
      WatToken t;
      uint64_t func;
      uint64_t index;
    };

    KHASH_DECLARE(indexname, utility::StringRef, varuint32);
    KHASH_DECLARE(tokens, utility::StringRef, WatTokenID);

    struct WatState
    {
      WatState(Environment& e, Module& mod);
      ~WatState();
      varuint32 GetJump(WatToken var);

      Environment& env;
      Module& m;
      Queue<DeferWatAction> defer;
      Stack<utility::StringRef> stack;
      kh_indexname_t* typehash;
      kh_indexname_t* funchash;
      kh_indexname_t* tablehash;
      kh_indexname_t* memoryhash;
      kh_indexname_t* globalhash;
    };

#define EXPECTED(t, e, err) if((t).Size() == 0 || (t).Pop().id != (e)) return assert(false), (err)

    int ParseWatModule(Environment& env, Module& m, uint8_t* data, size_t sz, utility::StringRef name);
    int WatString(ByteArray& str, utility::StringRef ref);
    void SkipSection(Queue<WatToken>& tokens, int count = 1);
    int WatInitializer(WatState& state, Queue<WatToken>& tokens, Instruction& op);
    void TokenizeWAT(Queue<WatToken>& tokens, const char* s, const char* end);
    int WatName(ByteArray& name, const WatToken& t);
    int WatModule(Environment& env, Module& m, Queue<WatToken>& tokens, utility::StringRef name, WatToken& internalname);
    size_t WatLineNumber(const char* start, const char* pos);
    int CheckWatTokens(ValidationError*& errors, Queue<WatToken>& tokens, const char* start);
    const char* GetTokenString(WatTokenID token);
      
    IR_FORCEINLINE int WatString(ByteArray& str, const WatToken& t)
    {
      if(t.id != TOKEN_STRING)
        return ERR_WAT_EXPECTED_STRING;
      return WatString(str, utility::StringRef{ t.pos, t.len });
    }

    IR_FORCEINLINE WatToken GetWatNameToken(Queue<WatToken>& tokens)
    {
      return (tokens.Peek().id == TOKEN_NAME) ? tokens.Pop() : WatToken{ TOKEN_NONE };
    }
  }
}

#endif