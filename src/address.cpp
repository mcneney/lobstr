#include <Rcpp.h>
#include "utils.h"
using namespace Rcpp;

int prim_refs_(SEXP x) {
  return NAMED(x);
}

// [[Rcpp::export]]
int prim_refs_(SEXP name, Environment env) {
  return prim_refs_(find_var(name, env));
}

// Address -------------------------------------------------

std::string prim_address_(SEXP x) {
  return tfm::format("%p", x);
}

// [[Rcpp::export]]
std::string prim_address_(SEXP name, Environment env) {
  return prim_address_(find_var(name, env));
}

void frame_addresses(SEXP frame, std::vector<std::string>* refs) {
  for(SEXP cur = frame; cur != R_NilValue; cur = CDR(cur)) {
    SEXP obj = CAR(cur);
    if (obj != R_UnboundValue)
      refs->push_back(prim_address_(obj));
  }
}
void hash_table_addresses(SEXP table, std::vector<std::string>* refs) {
  int n = Rf_length(table);
  for (int i = 0; i < n; ++i)
    frame_addresses(VECTOR_ELT(table, i), refs);
}

std::vector<std::string> prim_addresses_(SEXP x) {
  int n = Rf_length(x);
  std::vector<std::string> out;

  switch(TYPEOF(x)) {
  case STRSXP:
    for (int i = 0; i < n; ++i) {
      out.push_back(prim_address_(STRING_ELT(x, i)));
    }
    break;

  case VECSXP:
    for (int i = 0; i < n; ++i) {
      out.push_back(prim_address_(VECTOR_ELT(x, i)));
    }
    break;

  case ENVSXP: {
    bool isHashed = HASHTAB(x) != R_NilValue;
    if (isHashed) {
      hash_table_addresses(HASHTAB(x), &out);
    } else {
      frame_addresses(FRAME(x), &out);
    }
    break;
  }

  default:
    Rcpp::stop("type not supported");
  }

  return out;
}

// [[Rcpp::export]]
std::vector<std::string> prim_addresses_(SEXP name, Environment env) {
  return prim_addresses_(find_var(name, env));
}
