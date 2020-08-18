#include "cfg.h"

using namespace lang::compiler;

void ir::cfg::block::add_successor(std::shared_ptr<block> succ)
{
	succ->predecessors.push_back(weak_from_this());
	successors.push_back(std::move(succ));
}