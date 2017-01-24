const util = require('./util');
var remote; // This need to be loaded after the database connects

// Wait for database connection
require('./models')
    .connect()
    .then(() => remote = require('./remote'))
    .then(main);

function main() {
    // Put program instructions here
    demo();
}

/**
 * Example / Test
 * Put some pictures in NEW_USER_PHOTO_PATH and they will be uploaded 
 * as a new profile, then downloaded again (along with any others) into 
 * PHOTO_STORE_PATH, an identification index file will be generated at 
 * USERS_FILE_PATH.
 */
function demo() {
    // Create profile
    var newUserPhotos = util.readNewUserPhotos();
    remote.createProfile(newUserPhotos)
        .then(() => {
            // Download missing profiles
            var localProfiles = util.getLocalProfiles();
            return remote.pullLoginFiles(localProfiles);
        })
        .catch(console.error)
        .then(() => process.exit());
}