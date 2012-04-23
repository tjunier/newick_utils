function start_tree()
	n = 0
end

function node()
	s = string.format("N%d [label=\"%s\"]\n", n, lbl)
	io.write(s)
	n = n + 1 
end
	
