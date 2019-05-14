function path_joinMany(basePath, appendees, sep) {
    let out = [];
    for(let i = 0; i < appendees.length; i++) {
        out[i] = basePath + sep + appendees[i];
    }
    return out;
}

function join(sep, elements) {
    let out = "";
    for(let i = 0; i < elements.length; i++) {
        out = out + (i==0?"":sep) + elements[i];
    }
    return out;
}

function prepend(token, elements) {
    let out = token;
    out = out + join(token, elements);
    return out;
}