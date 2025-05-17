#pragma once

namespace DarkMatterVM 
{
namespace Translator 
{

/**
 * @brief 이항 연산자 타입
 */
enum class BinaryOpType 
{
	Add,        // +
	Subtract,   // -
	Multiply,   // *
	Divide,     // /
	Modulo,     // %
	
	Equal,      // ==
	NotEqual,   // !=
	Greater,    // >
	Less,       // <
	GreaterEq,  // >=
	LessEq,     // <=
	
	LogicalAnd, // &&
	LogicalOr,  // ||
	
	BitwiseAnd, // &
	BitwiseOr,  // |
	BitwiseXor, // ^
	ShiftLeft,  // <<
	ShiftRight  // >>
};

/**
 * @brief 단항 연산자 타입
 */
enum class UnaryOpType 
{
	Negate,     // -
	LogicalNot, // !
	BitwiseNot, // ~
	PreIncrement,  // ++x
	PostIncrement, // x++
	PreDecrement,  // --x
	PostDecrement  // x--
};

/**
 * @brief 할당 연산자 타입
 */
enum class AssignmentOpType
{
	Simple,     // =
	Add,        // +=
	Subtract,   // -=
	Multiply,   // *=
	Divide,     // /=
	Modulo,     // %=
	BitwiseAnd, // &=
	BitwiseOr,  // |=
	BitwiseXor, // ^=
	ShiftLeft,  // <<=
	ShiftRight  // >>=
};

} // namespace Translator
} // namespace DarkMatterVM 