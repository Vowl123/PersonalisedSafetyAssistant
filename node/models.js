/**
 * This file contains all the Mongoose Schema for connecting to the remote DB.
 */

const fs = require('fs');
const mongoose = require('mongoose');
const gridfs = require('mongoose-gridfs');
const DB_URL = require('./config').DB_URL;

console.log('\nConnecting to database at: ' + DB_URL);
mongoose.connect(DB_URL, function (err) {
    if (err) return console.error('Failed to connect to database!\n', err);
    return console.log('Connected successfully');
});


// Photo Schema
//-----------------------------------------------------------------------------
const photos = gridfs({
    collection: 'faceImages',
    model: 'Photo'
});
exports.photo = mongoose.model('Photo', photos.schema);


// User Profile Schema
//-----------------------------------------------------------------------------
const profileSchema = new mongoose.Schema({
    dateCreated: {type: Date, default: Date.now},
    dateActive: {type: Date, default: Date.now},
    photos: [{type: mongoose.Schema.Types.ObjectId, ref: 'Photo'}]
        // TODO - Videos and quirk logs
});
exports.profile = mongoose.model('Profile', profileSchema);