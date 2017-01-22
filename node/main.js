// Wait for database connection
require('./models').connect(function () {
    const fs = require('fs');
    const util = require('./util');

    const localProfiles = [];

//    // TESTING - Creates profile from images in loginFiles/new
//    const newProfileImages =
//        fs.readdirSync('../loginFiles/new')
//        .map(fname => '../loginFiles/new/' + fname);
//    util.createProfile(newProfileImages);

//    // Downloads all profiles not in localProfiles
//    util.pullLoginFiles(localProfiles);
});