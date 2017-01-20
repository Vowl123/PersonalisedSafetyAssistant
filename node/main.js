
const fs = require('fs');
const util = require('./util');

const localProfiles = [];


// TESTING
const newProfileImages = fs.readdirSync('../loginFiles/new');
util.createProfile(newProfileImages)
    .then(console.log)
    .catch(console.error);

//util.pullLoginFiles(localProfiles);