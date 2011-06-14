
function start_tree() node_num = 0; leaf_num = 0; end

function node()
	node_num = node_num + 1
	if l then leaf_num = leaf_num + 1 end
end

function stop_tree() 
	print(string.format("%d nodes (%d leaves)",
		node_num, leaf_num))
end
