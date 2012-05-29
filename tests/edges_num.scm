(
 (start-tree (begin (define node-num 0) (define leaf-num 0)))
 (#t (set! node-num (+ 1 node-num)))
 (l  (set! leaf-num (+ 1 leaf-num)))
 (end-tree (format #t "~d nodes (~d leaves)~%" node-num leaf-num))
)
