# there ain't no loops here bruh, use recursion!

def factorial[x] -> {
    if (x == 0) {
        1
    } else {
        x * factorial[x - 1]
    }
}

def main[] -> {
    print[factorial[20]]
    0
}
