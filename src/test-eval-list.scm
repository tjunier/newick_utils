(use-modules (ice-9 rdelim))

(lambda (lst)    ; L: (test1 test2 ... testn)
	(letrec ((eval-tests (lambda (L)
		(if (null? L)
      #f
      (let* ((test (car L))            ; test: (clause action)
             (clause (car test))
             (action (cadr test)))
        (if (primitive-eval clause)
            (begin (primitive-eval action) (or (eval-tests (cdr L)) #t))
            (or (eval-tests (cdr L)) #f )))))))
	(eval-tests lst)))


(define categorize-tests
 	(lambda (lst)
	 (letrec ((categ-tests
						 (lambda (L)
							(if (null? L) '()
							 (let* ((test (car L))
											(clause (car test)))
								(case (clause)
								 (('begin) (format #t "begin ~a~%" test))
								 (else		 (format #t "main ~a~%" test))))))))
		(categ-tests lst))))

;; Evaluates a list of tests. A test is a (clause action) pair. Iff the clause
;; is #t, the action gets evaluated. The function returns #t iff at least one
;; test is #t.

(define (test-eval-list L)	; L: (test1 test2 ... testn)
	(if (null? L)
		#f
	 	(let* ((test (car L))		; test: (clause action)
					 (clause (car test))
					 (action (cadr test)))
		 (if (primitive-eval clause)
			 (begin (primitive-eval action) (or (test-eval-list (cdr L)) #t))
			 (or (test-eval-list (cdr L)) #f )))))

(define l1 '(
						((even? 2) (write-line 2))
						((even? 3) (write-line 3))
						((even? 4) (write-line 4))
					 ))

(define l2 '(
						((even? 2) (write-line 2))
						((even? 3) (write-line 3))
						((even? 4) (write-line 4))
						(list? '()	(/ 1 2))
					 ))

(define l3 '(
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						(#f	'())
						))
