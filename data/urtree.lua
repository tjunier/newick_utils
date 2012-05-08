
nodes = {}
edges = {}

function start_tree()
	n = 0
	print("strict graph tree {")
end

function node()
	node_string = string.format("N%s [label=\"%s\"]", N.id, lbl)
	table.insert(nodes, node_string)
	if not r then
		edge_string = string.format("N%s -- N%s", N.id, N.par.id)
		table.insert(edges, edge_string)
	end
	n = n + 1 
end

function stop_tree()
	for i,v in ipairs(nodes) do print(v) end
	for i,v in ipairs(edges) do print(v) end
	print("}")
end
	
