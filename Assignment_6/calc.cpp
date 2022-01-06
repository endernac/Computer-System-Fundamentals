#include <unordered_map>
#include <deque>
#include <string>
#include <sstream>
#include <pthread.h>
#include "calc.h"

struct Calc {
    // hashtable to store variables
    std::unordered_map<std::string, int> *vars;
    // mutex to make table threadsafe
    pthread_mutex_t lock;
};

class CalcImpl : public Calc {
private:
    // functions splits up a string based on whitespace and stores tokens in deque
    std::deque<std::string> tokenize(const std::string &expr) {
        std::deque<std::string> dec;
        std::stringstream s(expr);

        std::string tok;
        while (s >> tok) {
            dec.push_back(tok);
        }

        return dec;
    }

    // takes in a string and checks if it is and integer. Note that first char can be '-'
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

    // takes in a string and checks if all chars are A-Z or a-z
    bool is_var(const std::string &s) {
        std::string::const_iterator it = s.begin();
        while(it != s.end() && std::isalpha(*it)) {
            it++;
        }
        return !s.empty() && it == s.end();
    }


    // takes in a string and puts its numerical value into *r if possible
    int value(const std::string &operand, int *r) {
        // it is a number so evaluate it as usual
        if(is_num(operand)) {
            *r = std::stoi(operand);
        } else if(is_var(operand)) {
            //it is a valid, known variable so get value from table
            if (vars->find(operand) != vars->end()) {
                *r = (*vars)[operand];
            } else {    // valid name but not in table
                return 0;
            }
        } else {    // not valid name and not a number
            return 0;
        }

        return 1;
    }

    // this function expects a string of the form 'a' or 'a op b' where a and b are either numbers
    // or valid, known variables and op is an operation.
    int eval(const std::deque<std::string> &expr, int *r) {
        if(expr.size() == 1 && value(expr[0], r)) { // string of the form 'a', return 1
            return 1;
        } else if(expr.size() == 3) { // string of the form 'a op b'
            int v1 = 0;
            int v2 = 0;

            if(!value(expr[0], &v1) || !value(expr[2], &v2)) { // get a and b
                return 0;
            }

            // get result based on op. Make sure not to divide by 0
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

            // operation suceeded
            return 1;
        }

        // string not of correct form
        return 0;
    }


public:
    // constructor - init map and mutex
    CalcImpl() {
        vars = new std::unordered_map<std::string, int>;
        pthread_mutex_init(&lock, NULL);
    }

    // destructor - destroy map and mutex
    ~CalcImpl() {
        delete vars;
        pthread_mutex_destroy(&lock);
    }

    // evaluate expression
    int evalExpr(const char *expr, int *r) {
        if(expr[0] == '\0') { // make sure string isn't empty
            return 0;
        }

        // remove line ending characters from string
        std::string s(expr);
        for(int i = 0; i < 2; i++) {
            if (!s.empty() && (s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n')) {
                s.erase(s.size() - 1);
            }
        }

        // tokenize the string and make a variable to hold return val
        std::deque<std::string> symb = tokenize(s);
        int flag = 0;

        // critical section - lock mutex. Make sure code does not return while mutex locked
        pthread_mutex_lock(&lock);
        // if string is of the form 'c = expr', slpit string on '=' and send 'expr' to eval
        if(symb.size() > 1 && symb[1].compare("=") == 0) {
            std::string v = symb.front();
            symb.pop_front();
            symb.pop_front();
            if(eval(symb, r)) {
                (*vars)[v] = *r;
                flag++;
            }
        } else if(eval(symb, r)) {
            // if string is of form 'expr' with no '=' then just evaluate directly
            flag++;
        }
        // remember to unloc
        pthread_mutex_unlock(&lock);
        
        return flag;
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

