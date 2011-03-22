(
 (not l)	; not a leaf
 (begin
	(use-modules (srfi srfi-1))
	(let* ((children (children-list N))
					(nb-kids (children-count N))
					(sum (fold + 0.0 (map (lambda (n) (children-count n)) children)))
					(sqr (lambda (x) (* x x)))
					(sum-sq (fold
									 	+
									 	0.0
										(map (lambda (n) (sqr (children-count n))) children)))
					(avg (/ sum nb-kids))
					(stdev (- (/ sum-sq nb-kids) (sqr avg)))
					)
	 (format #t "~a: avg ~f kids (stdev: ~f)~%" (lab N) avg stdev)
	)
 )
)

