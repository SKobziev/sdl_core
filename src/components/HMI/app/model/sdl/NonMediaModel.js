/**
 * @name MFT.SDLNonMediaModel
 * @desc Concrate model for SDL NonMedia application
 * @category Model
 * @filesource app/model/sdl/NonMediaModel.js
 * @version 1.0
 * @author Artem Petrosyan
 */

MFT.SDLNonMediaModel = MFT.SDLAppModel.extend( {

    init: function() {

        this._super();

        // init properties here
        this.set( 'appInfo', Em.Object.create( {
            field1: '<field1>',
            field2: '<field2>',
            field3: '<field3>',
            field4: '<field4>',
            mainImage: 'images/sdl/audio_icon.jpg',
            image: '',
            customPresets: []
        } ) );

        // this.set('appIcon', 'images/info/info_leftMenu_apps_ico.png'),

        this.set( 'commandsList', [] );
        this.set( 'softButtons', [] );
    },

    /**
     * Method calls from model method to deactivate application
     * 
     * @param {Number}
     */
    deactivateApp: function() {
        MFT.SDLController.getApplicationModel( this.currentAppId ).deactivateApp();
    },

    /**
     * Method hides sdl activation button and sdl application
     * 
     * @param {Number}
     */
    onDeleteApplication: function( appId ) {
        MFT.NonMediaController.onDeleteApplication( appId );
    },

    /**
     * Activate current application model
     */
    turnOnSDL: function() {
        MFT.NonMediaController.activateApp( this );
    },

    /**
     * Method to clear App OverLay
     */
    clearAppOverLay: function() {

        clearInterval( this.timer );
        this.appInfo.set( 'field1', '' );
        this.appInfo.set( 'field2', '' );
        this.appInfo.set( 'field3', '' );
        this.appInfo.set( 'field4', '' );
        this.appInfo.set( 'alignment', '' );
        this.appInfo.set( 'mainImage', 'images/sdl/audio_icon.jpg' );
        this.updateSoftButtons();
        for( i = 0; i < 6; i++ ){
            this.appInfo.set( 'customPresets.' + i, '' );
        }

    },

    /**
     * Applin UI Show handler
     * 
     * @param {Object}
     */
    onSDLUIShow: function( params ) {
        this.appInfo.set( 'field1', params.mainField1 );
        this.appInfo.set( 'field2', params.mainField2 );
        this.appInfo.set( 'field3', params.mainField3 );
        this.appInfo.set( 'field4', params.mainField4 );
        if( params.graphic ){
            this.appInfo.set( 'mainImage', params.graphic );
        }else{
            this.appInfo.set( 'mainImage', 'images/sdl/audio_icon.jpg' );
        }

        if( params.customPresets ){
            this.appInfo.set( 'customPresets', params.customPresets );
        }

        // Get soft buttons if exists
        if( params.softButtons ){
            this.updateSoftButtons( params.softButtons );
        }
    },

    sdlSetMediaClockTimer: function() {
        return;
    }
} );