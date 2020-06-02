<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the
 * installation. You don't have to use the web site, you can
 * copy this file to "wp-config.php" and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * MySQL settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://wordpress.org/support/article/editing-wp-config-php/
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define( 'DB_NAME', 'wordpress' );

/** MySQL database username */
define( 'DB_USER', 'user' );

/** MySQL database password */
define( 'DB_PASSWORD', '42' );

/** MySQL hostname */
define( 'DB_HOST', 'localhost' );

/** Database Charset to use in creating database tables. */
define( 'DB_CHARSET', 'utf8mb4' );

/** The Database Collate type. Don't change this if in doubt. */
define( 'DB_COLLATE', '' );

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define( 'AUTH_KEY',         'Y%faZfToPPPS&}t<g9n}dpfKnPQkbl9)bW;Tx1Dl_UAbpv@C)%iW]V?r =pFu,:;' );
define( 'SECURE_AUTH_KEY',  'gf,wk>h}$f_y%pVY>^Lr}-0&Cn.[3DGpW5i!3O~$8Ry=toYc<MInMY1(PF]5Otyf' );
define( 'LOGGED_IN_KEY',    '0z%4dHB%s666yyI3]iy5[u8V@-V>h0@]H*5Q08F+5wOq!gGvg$%-Hq`%FE^ Slfr' );
define( 'NONCE_KEY',        '>8(T:4E:%)(za7PHwD_(=u{~m;95%GVrOkHB^&N!Q|5&LCj6ZYuv#JjTP:(B!rM;' );
define( 'AUTH_SALT',        '61ho<Ji|O!WvS ,aJzkv6#liApVE JHJ3`(`Lm=}Eb.|!AT -Os#]# -gBbwP9|K' );
define( 'SECURE_AUTH_SALT', '(=X)S:^yD=(R%=d1>/zk-/P%5,-S:vV,;ok~{9I>]HD=po{-G+$lSoYK*0yl6f*U' );
define( 'LOGGED_IN_SALT',   'X83n<UmL?@$=HjG L,B:NG_0D/^hOO+Z*^]^sL`h6o=8(%Bm%GhH;Kh^^wxIq%l*' );
define( 'NONCE_SALT',       'JM>B^_FON^i[*8&D0[TSo.U4iM=/|*161eAzls,^v2lT|S[{+dJv1Yn;Ej&VUm2o' );

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the documentation.
 *
 * @link https://wordpress.org/support/article/debugging-in-wordpress/
 */
define( 'WP_DEBUG', false );

/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if ( ! defined( 'ABSPATH' ) ) {
	define( 'ABSPATH', __DIR__ . '/' );
}

/** Sets up WordPress vars and included files. */
require_once ABSPATH . 'wp-settings.php';

