(define (setup)
  (background 0 0 0))

(define (draw)
  (if *mouse-1-pressed*
      (fill 255 0 0 .2)
      (fill 0 0 255 .2))
  (ellipse *mouse-x* *mouse-y* 30 30))
