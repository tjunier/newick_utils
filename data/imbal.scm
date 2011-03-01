(
 (not l)	; not a leaf
 (begin
	 (srfi srfi-1)
	 (let* ((children (children-list N))
					(sum (fold + 0 (map (lambda (n) (children-count n)) children))))
	 (display sum)
	 )
	 )
)
