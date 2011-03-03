(use-modules (ice-9 rdelim))

(define (test-eval-list L)
	(if (null? L) #t
	 (let* ((test (car L))
					(clause (car test))
					(action (cadr test)))
		(if (primitive-eval clause)
		 (begin
			(primitive-eval action)
			(and #t (test-eval-list (cdr L))))
		 (and #t (test-eval-list (cdr L)))))))

(define l1 '(
						((even? 2) (write-line #t))
						((even? 3) (write-line #f))
						((even? 4) (write-line #t))
					 ))
