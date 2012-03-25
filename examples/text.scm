(define (setup)
  (background 0 0 0))

(define (draw)
  (background 0 0 0)
  (fill 100 50 250 1)
  (fill 255 0 0 1)
  (rect *mouse-x* *mouse-y* 10 20))
