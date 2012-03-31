(define (rand n)
  (remainder (random-next) n))

(define (setup)
  (set-fps 30)
  (background 0 0 0))

(define (draw)
  (let ((r (rand 255))
        (g (rand 255))
        (b (rand 255)))
    (background r g b)
    (fill b g r 1)
    (rect (- *height* *mouse-y*) (- *width* *mouse-x*) 20 20)))
