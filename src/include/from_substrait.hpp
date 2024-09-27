#pragma once

#include <string>
#include <unordered_map>
#include "substrait/plan.pb.h"
#include "duckdb/main/connection.hpp"
#include "duckdb/common/shared_ptr.hpp"

namespace duckdb {

class SubstraitToDuckDB {
public:
	SubstraitToDuckDB(Connection &con_p, const string &serialized, bool json = false);
	//! Transforms Substrait Plan to DuckDB Relation
	shared_ptr<Relation> TransformPlan();

private:
	//! Transforms Substrait Plan Root To a DuckDB Relation
	shared_ptr<Relation> TransformRootOp(const substrait::RelRoot &sop);
	//! Transform Substrait Operations to DuckDB Relations
	shared_ptr<Relation> TransformOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformJoinOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformCrossProductOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformFetchOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformFilterOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformProjectOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformAggregateOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformReadOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformSortOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformSetOp(const substrait::Rel &sop);
	shared_ptr<Relation> TransformWriteOp(const substrait::Rel &sop);

	//! Transform Substrait Expressions to DuckDB Expressions
	unique_ptr<ParsedExpression> TransformExpr(const substrait::Expression &sexpr);
	static unique_ptr<ParsedExpression> TransformLiteralExpr(const substrait::Expression &sexpr);
	static unique_ptr<ParsedExpression> TransformSelectionExpr(const substrait::Expression &sexpr);
	unique_ptr<ParsedExpression> TransformScalarFunctionExpr(const substrait::Expression &sexpr);
	unique_ptr<ParsedExpression> TransformIfThenExpr(const substrait::Expression &sexpr);
	unique_ptr<ParsedExpression> TransformCastExpr(const substrait::Expression &sexpr);
	unique_ptr<ParsedExpression> TransformInExpr(const substrait::Expression &sexpr);
	unique_ptr<ParsedExpression> TransformNested(const substrait::Expression &sexpr);

	static void VerifyCorrectExtractSubfield(const string &subfield);
	static string RemapFunctionName(const string &function_name);
	static string RemoveExtension(const string &function_name);
	static LogicalType SubstraitToDuckType(const substrait::Type &s_type);
	//! Looks up for aggregation function in functions_map
	string FindFunction(uint64_t id);

	//! Transform Substrait Sort Order to DuckDB Order
	OrderByNode TransformOrder(const substrait::SortField &sordf);
	//! DuckDB Connection
	Connection &con;
	//! Substrait Plan
	substrait::Plan plan;
	//! Variable used to register functions
	unordered_map<uint64_t, string> functions_map;
	//! Remapped functions with differing names to the correct DuckDB functions
	//! names
	static const unordered_map<std::string, std::string> function_names_remap;
	static const case_insensitive_set_t valid_extract_subfields;
	vector<ParsedExpression *> struct_expressions;
};
} // namespace duckdb
