'use strict';

module.exports = function() {
    return [].slice.call(arguments, 0, -1).reduce(function(object, key) {
        return object[key];
    });
};
