
const fs = require('fs');
const path = require('path');
const photoDB = require('./models').photo;
const profileDB = require('./models').profile;
const config = require('./config');

const PHOTO_STORE_PATH = config.PHOTO_STORE_PATH;
const USERS_FILE_PATH = config.USERS_FILE_PATH;

exports.createProfile = createProfile;
exports.pullLoginFiles = pullLoginFiles;
exports.getProfiles = getProfiles;


// POST
//-----------------------------------------------------------------------------

/**
 * Returns a promise to create a profile with a set of user photos.
 * photos should be an array of filepaths to images of the user for login.
 * Resolves with the profile object.
 */
function createProfile(photos) {

    // TODO - expand on profile content
    var profile = profileDB();

    var tasks = photos.map(uploadPhoto);

    return Promise.all(tasks).then(photoIDs => {
        console.log('Saving profile with photoIDs:', photoIDs);
        profile.photos = photoIDs;
        profile.save((err, res) => {
            if (err) return Promise.reject(err);
            return Promise.resolve(res);
        });
    });
}

/**
 * Returns a promise to upload the image at filepath to the photo database.
 * Resolves with the id of the photo, or an error message.
 * Will always resolve (safe to use with Promise.All)
 */
function uploadPhoto(filepath) {
    return new Promise((resolve, reject) => {

        // Create document with Meta-data
        var photo = new photoDB({
            filename: path.basename(filepath),
            contentType: 'image'
        });

        // Upload file contents
        photo.write(
            fs.createReadStream(filepath),
            (err, res) => {
            // Always resolve even if failed to save,
            // this is nessesary for Promise.All to continue if one fails.
            resolve(err || res._id);
        });
    });
}



// GET
//-----------------------------------------------------------------------------

/**
 * Returns a promise to download all known profile images from databse, 
 * except those of knownProfiles.
 */
function pullLoginFiles(knownProfiles) {
    return getProfiles(knownProfiles)
        .then(profiles => Promise.all(profiles.map(downloadPhotos)))
        .catch(console.error);
}

/**
 * Returns a promise which fufills with an array of all profiles 
 * with an id not in ignoreIDs.
 */
function getProfiles(ignoreIDs) {
    var query = {_id: {$nin: ignoreIDs}};

    return new Promise((resolve, reject) => {
        profileDB.find(query, (err, res) => {
            if (err) reject(err);
            else resolve(res);
        });
    });
}

/**
 * Returns a promise to download all photos for profile 
 * and add them to the login file.
 * profile should be an instance of Profile schema.
 */
function downloadPhotos(profile) {
    var query = {_id: {$in: profile.photos}};

    return new Promise((resolve, reject) => {
        photoDB.find(query, (err, res) => {
            if (err) return reject(err);

            var tasks = [];
            for (var i = 0; i < res.length; i++) {
                tasks.push(savePhotoForLogin(profile._id, res[i]));
            }
            return Promise.all(tasks).then(resolve);
        });
    });
}

/**
 * Returns a promise to save a photo for login by profileID.
 * Will always resolve (safe to use with Promise.All)
 * photo should be an instance of Photo schema.
 */
function savePhotoForLogin(profileID, photo) {

    // Download / Save image
    return streamToFile(
        path.join(PHOTO_STORE_PATH, photo.filename),
        photo.read())

        // Append to profile index file
        .then(msg => {
            var nline = photo.filename + " " + profileID;
            fs.appendFile(USERS_FILE_PATH, nline);
            return msg;
        })
        // Always resolve even if failed to save,
        // this is nessesary for Promise.All to continue if one fails.
        .catch(Promise.resolve);
}

/**
 * Returns a promose to write data from stream to filepath.
 * filepath should include the filename and extension.
 */
function streamToFile(filepath, stream) {
    return new Promise((resolve, reject) => {

        var chunks = [];
        stream.on('data', chunks.push);

        stream.on('end', function () {
            var buffer = Buffer.concat(chunks);
            fs.writeFile(filepath, buffer, function (err) {
                if (err) reject(err);
                resolve('Saved Image: ' + filepath);
            });
        });
    });
}