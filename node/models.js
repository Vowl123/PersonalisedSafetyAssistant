/**
 * This file contains all the Mongoose Schema for connecting to the remote DB.
 */

const fs = require('fs');
const mongoose = require('mongoose');
const gridfs = require('mongoose-gridfs');
const DB_URL = require('./config').DB_URL;

mongoose.Promise = global.Promise;

exports.connect = () => {
    console.log('\nConnecting to database at: ' + DB_URL);
    return mongoose.connect(DB_URL).then(startup);
};

function startup() {
    console.log('Connected successfully');


    // Photo Schema
    //----------------------------------------------------------------------
    const photos = gridfs({
        collection: 'photos',
        model: 'Photo'
    });

    var photoSchema = photos.schema;
    exports.photo = mongoose.model('Photo', photoSchema);



    // User Profile Schema
    //--------------------------------------------------------------------
    const profileSchema = new mongoose.Schema({
        dateCreated: {type: Date, default: Date.now},
        dateActive: {type: Date, default: Date.now},
        photos: [{type: mongoose.Schema.Types.ObjectId, ref: 'Photo'}]
            // TODO - Videos and quirk logs
    });
    exports.profile = mongoose.model('Profile', profileSchema);


    return Promise.resolve;
}


