
node_data = {}
is_leaf = {}

function node()
	local nd = { node = N, num = -1, depth = d, height = -1,
		pid = -1, pruning_level = -1}
	if l then is_leaf[N.id] = true else is_leaf[N.id] = false end
	table.insert(node_data, nd)
end

function stop_tree()
	num_leaves = (#node_data + 1)/2
	-- num_inodes = #node_data - num_leaves
	print("NodeID", "ParentID", "PruningLevel", "Height", "LeafOrder", "Type")
	table.sort(node_data, function (a,b) return (a.depth < b.depth) end)
	depths = {}
	for i = 1, #node_data do depths[i] = node_data[i].depth end
	max_depth = -1
	for i = 1, #depths do 
		if depths[i] > max_depth then max_depth = depths[i]
		end
	end
	num = #node_data
	for i = 1, #node_data do
		local nd = node_data[i]
		local id = nd.node.id
		nd.height = max_depth - nd.depth
		nd.num = num
		num = num - 1
		if not is_leaf[id] then 
			nd.node.lbl = num	-- deleatur
			nd.pruning_level = i - 1
		else
			nd.pruning_level = num_leaves - 1
		end
	end
	nodes_by_id = {}
	for i = 1, #node_data do
		local nd = node_data[i]
		local id = nd.node.id
		nodes_by_id[id] = nd
	end
	table.sort(node_data, function (a,b) return (tonumber(a.node.lbl) > tonumber(b.node.lbl)) end)
	for i = #node_data, 1, -1 do
		local nd = node_data[i]
		local plev = nd.pruning_level
		local pid = nd.node.par.id
		local pnum
		if pid == "(nil)" then
			pnum = -1
		else
			local pnd = nodes_by_id[pid]
			pnum = pnd.num
		end
		local leaf_order = ""
		if 0.0 == nd.height then leaf_order = nd.node.lbl end
		print(nd.num, pnum, plev, string.format("%.2f", nd.height),
			leaf_order)
	end
end
