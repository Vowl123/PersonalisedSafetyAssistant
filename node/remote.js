
const fs = require('fs');
const path = require('path');
const config = require('./config');
const util = require('./util');
const photoDB = require('./models').photo;
const profileDB = require('./models').profile;

const PHOTO_STORE_PATH = config.PHOTO_STORE_PATH;
const USERS_FILE_PATH = config.USERS_FILE_PATH;
const log = util.log;

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

    if (!Array.isArray(photos) || !photos.length)
        return Promise.reject('Photos are required for profile creation');

    log('Creating profile with photos:\n', photos);

    // Create profile document
    var profile = profileDB();

    // Upload photos
    return Promise.all(photos.map(uploadPhoto))

        // ID or undefined
        .then(res => res.map(doc => doc !== undefined ? doc._id : undefined))

        // Attatch id's to to profile & save
        .then(ids => profile.photos = ids)
        .then(() => {
            // This is structured strangely due to .save 
            // being then-able but not a promise
            return profile.save().then(() => {
                log('Created profile:\n', profile);
                return profile;
            });
        });
}

/**
 * Returns a promise to upload the image at filepath to the photo database.
 * Resolves with the id of the photo, or an error message.
 * Will always resolve (safe to use with Promise.All)
 */
function uploadPhoto(filepath) {
    // Create document with Meta-data
    var photo = new photoDB({
        filename: path.basename(filepath),
        contentType: 'binary'
    });

    // Read file from disk
    var stream = fs.createReadStream(filepath);

    // Promise to upload file contents
    return new Promise(function (resolve, reject) {
        photo.write(stream, (err, res) => {
            // Always resolve
            if (err !== null) console.error(err);
            resolve(res || err);
        });
    });
}


// GET
//-----------------------------------------------------------------------------

/**
 * Returns a promise to download all known profile images from databse, 
 * except those of knownProfiles.
 * Resolves with an array of all newly cached profiles.
 */
function pullLoginFiles(knownProfiles) {
    log('Pulling missing profiles');
    return getProfiles(knownProfiles)
        .then(profiles => Promise.all(profiles.map(downloadPhotos)))
        .then(profiles => {
            log('Cached missing profiles:\n', profiles);
            return profiles;
        })
        .catch(console.error);
}

/**
 * Returns a promise which fufills with an array of all profiles 
 * with an id not in ignoreIDs.
 */
function getProfiles(ignoreIDs) {
    return profileDB.find({_id: {$nin: ignoreIDs}});
}

/**
 * Returns a promise to download all photos for profile 
 * and add them to the login file.
 * profile should be an instance of Profile schema.
 * Resolves with the same profile object
 */
function downloadPhotos(profile) {
    return photoDB.find({_id: {$in: profile.photos}})
        .then(
            res => Promise.all(res.map(
                    photo => savePhotoForLogin(profile._id, photo)))
        )
        .then(() => profile);
}

/**
 * Returns a promise to save a photo for login by profileID.
 * Will always resolve (safe to use with Promise.All)
 * photo should be an instance of Photo schema.
 */
function savePhotoForLogin(profileID, photo) {
    return new Promise((resolve, reject) => {
        util.ensureDir(PHOTO_STORE_PATH);

        var filepath = path.join(PHOTO_STORE_PATH, photo.filename);
        var writestream = fs.createWriteStream(filepath);

        // Download & Save
        photo.read().pipe(writestream);
        writestream.on('finish', () => {

            // Append to profile index file
            var nline = filepath + ";" + profileID + "\n";
            fs.appendFile(USERS_FILE_PATH, nline, resolve);
        });
    });
}