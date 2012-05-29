
nodes = {}
edges = {}
if nil == len_factor then
	len_factor = 1
end

function start_tree()
	n = 0
	print("strict graph tree {")
	print("graph [mode=KK,dimen=2]")
end

function node()
	if l then
		node_string = string.format("N%s [shape=plaintext,label=\"%s\"]", N.id, lbl)
	else
		node_string = string.format("N%s [shape=point]", N.id, lbl)
	end
	table.insert(nodes, node_string)
	if not r then
		edge_string = string.format("N%s -- N%s [len=%f,weight=10]",
			N.id, N.par.id, N.len * len_factor)
		table.insert(edges, edge_string)
	end
	n = n + 1 
end

function stop_tree()
	for i,v in ipairs(nodes) do print(v) end
	for i,v in ipairs(edges) do print(v) end
	print("}")
end
	
