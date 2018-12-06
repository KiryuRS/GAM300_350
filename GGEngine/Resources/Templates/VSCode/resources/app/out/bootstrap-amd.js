/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
"use strict";const loader=require("./vs/loader"),bootstrap=require("./bootstrap"),nlsConfig=bootstrap.setupNLS();loader.config({baseUrl:bootstrap.uriFromPath(__dirname),catchError:!0,nodeRequire:require,nodeMain:__filename,"vs/nls":nlsConfig,nodeCachedDataDir:process.env.VSCODE_NODE_CACHED_DATA_DIR}),(process.env.ELECTRON_RUN_AS_NODE||process.versions.electron)&&loader.define("fs",["original-fs"],function(o){return o}),nlsConfig.pseudo&&loader(["vs/nls"],function(o){o.setPseudoTranslation(nlsConfig.pseudo)}),exports.load=function(o,e,r){o&&loader([o],e=e||function(){},r=r||function(o){console.error(o)})};
//# sourceMappingURL=https://ticino.blob.core.windows.net/sourcemaps/bc24f98b5f70467bc689abf41cc5550ca637088e/core/bootstrap-amd.js.map
