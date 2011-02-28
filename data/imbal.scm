(
 (not l)	; not a leaf
 (let* ((children (children-list N)))
				(sum (foldr + 0 (map (lambda (n) (children-count n)) children)))
 )
)
