(import "balance_ow/balance_ow.bin" 'balanceowlib)

(load-native-lib balanceowlib)

; Switch Balance App to UART App
(if (= (conf-get 'app-to-use) 9) (conf-set 'app-to-use 3))

; Set to 1 to monitor some debug variables using the extension ext-euc-dbg
(define debug 1)

(if (= debug 1)
    (loopwhile t
        (progn
            (define setpoint (ext-balance-dbg 2))
            (define tt-filtered-current (ext-balance-dbg 3))
            (define integral (ext-balance-dbg 14))
            (sleep 0.1)
)))
