#include <unordered_map>
#include <deque>
#include <string>
#include <sstream>
#include "calc.h"

struct Calc {
    std::unordered_map<std::string, int> *vars;
};

class CalcImpl : public Calc {
private:
    std::deque<std::string> tokenize(const std::string &expr) {
        std::deque<std::string> dec;
        std::stringstream s(expr);

        std::string tok;
        while (s >> tok) {
            dec.push_back(tok);
        }

        return dec;
    }

    bool is_num(const std::string &s) {
        std::string::const_iterator it = s.begin();
        if(s.size() > 1 && s.at(0) == '-') {
            it++;
        }
        while(it != s.end() && std::isdigit(*it)) {
            it++;
        }
        return !s.empty() && it == s.end();
    }

    bool is_var(const std::string &s) {
        std::string::const_iterator it = s.begin();
        while(it != s.end() && std::isalpha(*it)) {
            it++;
        }
        return !s.empty() && it == s.end();
    }


    int value(const std::string &operand, int *r) {
        if(is_num(operand)) {
            *r = std::stoi(operand);
        } else if(is_var(operand)) {
            if (vars->find(operand) != vars->end()) {
                // variable name
                *r = (*vars)[operand];
            } else {
                return 0;
            }
        } else {
            return 0;
        }

        return 1;
    }

    int eval(const std::deque<std::string> &expr, int *r) {
        std::string s = expr[0];
        value(s, r);
        if(expr.size() == 1 && value(expr[0], r)) {
            return 1;
        } else if(expr.size() == 3) {
            int v1 = 0;
            int v2 = 0;

            if(!value(expr[0], &v1) || !value(expr[2], &v2)) {
                return 0;
            }

            if(expr[1].compare("+") == 0) {
                *r = v1 + v2;
            } else if(expr[1].compare("-") == 0) {
                *r = v1 - v2;
            } else if(expr[1].compare("*") == 0) {
                *r = v1 * v2;
            } else if(expr[1].compare("/") == 0 && v2 != 0) {
                *r = v1 / v2;
            } else {
                return 0;
            }

            return 1;
        }

        return 0;
    }


public:
    CalcImpl() {
        vars = new std::unordered_map<std::string, int>;
    }

    ~CalcImpl() {
        delete vars;
    }

    int evalExpr(const char *expr, int *r) {
        if(expr[0] == '\0') {
            return 0;
        }

        std::string s(expr);
        for(int i = 0; i < 2; i++) {
            if (!s.empty() && (s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n')) {
                s.erase(s.size() - 1);
            }
        }

        std::deque<std::string> symb = tokenize(s);
        eval(symb, r);

        if(symb.size() > 1 && symb[1].compare("=") == 0) {
            std::string v = symb.front();
            symb.pop_front();
            symb.pop_front();
            // TODO make sure v is a valid string (only has a-z & A-Z)
            if(eval(symb, r)) {
                (*vars)[v] = *r;
                return 1;
            }
        } else if(eval(symb, r)) {
            return 1;
        }
        
        return 0;
    }
};


extern "C" struct Calc *calc_create(void) {
    return new CalcImpl();
}

extern "C" void calc_destroy(struct Calc *calc) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    delete obj;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    return obj->evalExpr(expr, result);
}

