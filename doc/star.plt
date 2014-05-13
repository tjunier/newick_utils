set terminal postscript color eps "Helvetica" 18
set output 'star.eps'
set title 'Number of Nodes as a function of Depth in star'
set xlabel 'Tree Depth'
set ylabel '# Nodes'
set border back
set label "normalized area: .97      " at 8.0000,1.5 right front
plot [] [1:] 'star.cvd' with steps linewidth 4 title ''
