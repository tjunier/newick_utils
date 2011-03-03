(
 (begin-tree (define node-num 0))
 (#t (begin (format #t "counting ~a~%" (lab N)) (set! node-num (+ 1 node-num))))
 (end-tree (format #t "~d nodes~%" node-num))
)
