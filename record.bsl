record Point {
    x,
    y
}

def point_make[x, y] -> {
    record Point { x, y }
}

def main[] -> {
    let [points] -> {
        points -> point_make[34, point_make[35, 36]]
    }

    print[points]
    0
}
