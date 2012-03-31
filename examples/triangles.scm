(define (rand n)
  (remainder (random-next) n))

(define (rand-float n)
  (let* ((den (+ 1 (random-next)))
         (num (remainder (random-next) den)))
    (* n (/ num den))))

(define (setup)
  (background 0 0 0))

(define (draw)
  (fill (rand 255) (rand 255) (rand 255) (rand-float 1))
  (triangle (rand *width*) (rand *height*) 
            (rand *width*) (rand *height*)
            (rand *width*) (rand *height*)))


