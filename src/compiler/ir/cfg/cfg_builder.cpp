#include "cfg_builder.h"

#include <unordered_map>
#include <stdexcept>

using namespace seam::compiler;

struct cfg_visitor : ir::ast::visitor
{
	std::vector<std::pair<ir::ast::statement::function_definition*, std::shared_ptr<ir::cfg::block>>>& function_blocks;
	std::shared_ptr<ir::cfg::block> current;

	cfg_visitor(std::vector<std::pair<ir::ast::statement::function_definition*, std::shared_ptr<ir::cfg::block>>>& function_blocks) :
		function_blocks(function_blocks) {}

	bool visit(ir::ast::node* node) override
	{
		return true;
	}

	bool visit(ir::ast::statement::restricted_block* node) override
	{
		return true;
	}

	bool visit(ir::ast::statement::statement* node)
	{
		if (!current)
		{
			throw std::runtime_error("oh no");
		}
		current->body.push_back(node);
		return false;
	}

	bool visit(ir::ast::statement::block* node)
	{
		node->visit_children(this);
		return false;
	}

	bool visit(ir::ast::statement::function_definition* node)
	{
		function_blocks.emplace_back(node, current = std::make_shared<ir::cfg::block>());
		node->visit_children(this);
		current.reset();
		return false;
	}
};

cfg_builder::function_block_map cfg_builder::cfg_builder::build()
{
	std::vector<std::pair<ir::ast::statement::function_definition*, std::shared_ptr<ir::cfg::block>>> function_blocks;
	cfg_visitor visitor{ function_blocks };
	root->visit(&visitor);

	function_block_map block;
	
	return block;//function_blocks;
}