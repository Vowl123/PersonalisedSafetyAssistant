
const fs = require('fs');
const path = require('path');
const config = require('./config');

const NEW_USER_PHOTO_PATH = config.NEW_USER_PHOTO_PATH;
const USERS_FILE_PATH = config.USERS_FILE_PATH;
const LOG = true;

const log = LOG ? console.log : function () {};

exports.getLocalProfiles = getLocalProfiles;
exports.readNewUserPhotos = readNewUserPhotos;
exports.ensureDir = ensureDir;
exports.log = log;


/**
 * Create targetDir if not exists.
 */
function ensureDir(targetDir) {
    targetDir.split('/').forEach((dir, index, splits) => {
        var parent = splits.slice(0, index).join('/');
        var dirPath = path.resolve(parent, dir);
        if (!fs.existsSync(dirPath)) fs.mkdirSync(dirPath);
    });
}

/**
 * Returns an array of all unique profileID's found in the local users file.
 */
function getLocalProfiles() {
    // Read index file
    var text = '';
    try {
        text = fs.readFileSync(USERS_FILE_PATH, 'utf8');
    } catch (err) {
        var failed = true;
    }
    // Check if none found (or failed to read file)
    if (!text.length || failed) {
        log('Found no local profiles');
        return [];
    }
    // Split into array of lines, where each line is [filepath, id]
    var fragments = text.split('\n').map(line => line.split(';'));
    // Get an array of just the id's
    var ids = fragments.map(arr => arr[1]);
    // Return array of unique id's
    var uniqueIDs = Array.from(new Set(ids));
    log('Found local profiles:\n', uniqueIDs);
    return uniqueIDs;
}

/**
 * Returns an array of filepaths to all files in the NEW_USER_PHOTO_PATH.
 */
function readNewUserPhotos() {
    log('Reading photos from ' + NEW_USER_PHOTO_PATH);
    ensureDir(NEW_USER_PHOTO_PATH);
    return fs
        .readdirSync(NEW_USER_PHOTO_PATH)
        .map(fname => path.join(NEW_USER_PHOTO_PATH, fname));
}