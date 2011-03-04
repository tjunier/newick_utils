(use-modules (ice-9 rdelim))
(use-modules (srfi srfi-1))

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


;; Partitions the test list (of the form (test1 ... testn), where each test is
;; of the form (clause action)) into 5 lists based on the clause. The clause
;; can be either a keyword (begin, begin-tree, end, end-tree) or something
;; else. Tests with a keyword clause are performed at the matching phase (e.g.,
;; tests with 'begin-tree' just before visiting the tree, etc); other tests are
;; performed at each node in the tree.

(define partition-test-list
	(lambda (lst)
		(let ((begin-test-list 				'())
					(begin-tree-test-list 	'())
					(within-tree-test-list	'())
					(end-test-list					'())
					(end-tree-test-list			'()))

			(for-each (lambda (test)
									(cond
										((eq? (car test) 'begin)
										 (set! begin-test-list (cons test begin-test-list)))
										((eq? (car test) 'begin-tree)
										 (set! begin-tree-test-list (cons test begin-tree-test-list)))
										((eq? (car test) 'end)
										 (set! end-test-list (cons test end-test-list)))
										((eq? (car test) 'end-tree)
										 (set! end-tree-test-list (cons test end-tree-test-list)))
										(else
										 (set! within-tree-test-list (cons test within-tree-test-list)))))

								lst)

			(list (reverse begin-test-list)
						(reverse begin-tree-test-list)
						(reverse within-tree-test-list)
						(reverse end-tree-test-list)
						(reverse end-tree-test-list)))))

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

(define l4 '(
						 (begin (define num-trees 0))
						 (begin-tree (begin (define num-nodes 0)
																(define num-leaves 0)
																(set! num-trees (+ 1 num-trees))))
						 (#t (set! num-nodes (+ 1 num-nodes)))
						 (l  (set! num-leaves (+ 1 num-leaves)))
						 (end-tree (format "# nodes: ~d~%" num-nodes))
						 (end (format #t "# trees: ~d~%" num-trees))
						 ))
