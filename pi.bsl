# pi approximation using nilakantha series.

def pi[iterations, result, sign, divisor] -> {
    if (iterations == 0) {
        result
    } else {
        let [next_term] -> {
            next_term -> 4.0 * (1.0 / (divisor * (divisor + 1.0) * (divisor + 2.0))) * sign
        }

        let [_] -> {
            result -> result + next_term,
            sign -> sign * -1.0,
            divisor -> divisor + 2.0
        }

        pi[iterations - 1, result, sign, divisor]
    }
}

def main[] -> {
    print[ pi[1000, 3.0, 1.0, 2.0] ]
    0
}
