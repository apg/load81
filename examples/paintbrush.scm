(define color '(255 0 0 .1))

(define (setup)
  (background 255 255 255))

(define (draw)
  (let ((size (if *mouse-1-pressed* 20 10)))
    (apply fill color)
    (ellipse *mouse-x* *mouse-y* size size)))