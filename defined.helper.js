'use strict';

module.exports = function() {
    return [].slice.call(arguments, 0, -1).every(function(value) {
        return value !== undefined;
    });
};
