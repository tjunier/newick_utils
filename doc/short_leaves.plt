set terminal postscript color eps "Helvetica" 18
set output 'short_leaves.eps'
set title 'Number of Nodes as a function of Depth in short_leaves'
set xlabel 'Tree Depth'
set ylabel '# Nodes'
set border back
set label "normalized area:       " at 8,1.5 right front
plot [] [1:] 'short_leaves.cvd' with steps linewidth 4 title ''
