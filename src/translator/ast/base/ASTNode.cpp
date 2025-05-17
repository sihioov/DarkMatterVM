#include "ASTNode.h"

namespace DarkMatterVM 
{
namespace Translator 
{

void ASTNode::SetLocation(int line, int column) 
{
	_line = line;
	_column = column;
}

} // namespace Translator
} // namespace DarkMatterVM 