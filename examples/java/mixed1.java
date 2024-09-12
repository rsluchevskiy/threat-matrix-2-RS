/*
*    ------ BEGIN LICENSE ATTRIBUTION ------
*    
*    Portions of this file have been appropriated or derived from the following project(s) and therefore require attribution to the original licenses and authors.
*    
*    Project: http://spring.io/projects/spring-security
*    Release: https://github.com/spring-projects/spring-security/releases/tag/5.7.12
*    Source File: Base64.java
*    
*    Copyrights:
*      copyright 2002-2016 the original author or authors
*    
*    Licenses:
*      Apache License 2.0
*      SPDXId: Apache-2.0
*    
*    
*    Project: https://projects.spring.io/spring-boot
*    Release: https://github.com/spring-projects/spring-boot-data-geode/releases/tag/1.3.0.RELEASE
*    Source File: ObjectUtils.java
*    
*    Copyrights:
*      copyright 2020 the original author or authors
*    
*    Licenses:
*      Apache License 2.0
*      SPDXId: Apache-2.0
*    
*    
*    Project: https://spring.io/projects/spring-boot
*    Release: https://github.com/spring-projects/spring-boot/releases/tag/v2.6.15
*    Source File: Connection.java
*    
*    Copyrights:
*      copyright 2012-2021 the original author or authors
*    
*    Licenses:
*      Apache License 2.0
*      SPDXId: Apache-2.0
*    
*    
*    Project: https://spring.io/projects/spring-authorization-server
*    Release: https://github.com/spring-projects/spring-authorization-server/releases/tag/0.3.1
*    Source File: OAuth2TokenIntrospection.java
*    
*    Copyrights:
*      copyright 2020-2021 the original author or authors
*    
*    Licenses:
*      Apache License 2.0
*      SPDXId: Apache-2.0
*    
*    Auto-attribution by Threatrix, Inc.
*    
*    ------ END LICENSE ATTRIBUTION ------
*/
private class mixed1 {

// https://github.com/spring-projects/spring-authorization-server/blob/0.1.2/oauth2-authorization-server/src/main/java/org/springframework/security/oauth2/core/OAuth2TokenIntrospection.java
private void validate(){
        Assert.notNull(this.claims.get(OAuth2TokenIntrospectionClaimNames.ACTIVE),"active cannot be null");
        Assert.isInstanceOf(Boolean.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.ACTIVE),"active must be of type boolean");
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.SCOPE)){
        Assert.isInstanceOf(List.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.SCOPE),"scope must be of type List");
        }
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.EXP)){
        Assert.isInstanceOf(Instant.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.EXP),"exp must be of type Instant");
        }
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.IAT)){
        Assert.isInstanceOf(Instant.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.IAT),"iat must be of type Instant");
        }
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.NBF)){
        Assert.isInstanceOf(Instant.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.NBF),"nbf must be of type Instant");
        }
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.AUD)){
        Assert.isInstanceOf(List.class,this.claims.get(OAuth2TokenIntrospectionClaimNames.AUD),"aud must be of type List");
        }
        if(this.claims.containsKey(OAuth2TokenIntrospectionClaimNames.ISS)){
        validateURL(this.claims.get(OAuth2TokenIntrospectionClaimNames.ISS),"iss must be a valid URL");
        }
        }

// Taken from: https://github.com/spring-projects/spring-security/blob/5.5.6/crypto/src/main/java/org/springframework/security/crypto/codec/Base64.java
private static byte[] encodeBytesToBytes(byte[] source, int off, int len, int options) {

		if (source == null) {
			throw new NullPointerException("Cannot serialize a null array.");
		} // end if: null

		if (off < 0) {
			throw new IllegalArgumentException("Cannot have negative offset: " + off);
		} // end if: off < 0

		if (len < 0) {
			throw new IllegalArgumentException("Cannot have length offset: " + len);
		} // end if: len < 0

		if (off + len > source.length) {
			throw new IllegalArgumentException(String.format(
					"Cannot have offset of %d and length of %d with array of length %d", off, len, source.length));
		} // end if: off < 0

		boolean breakLines = (options & DO_BREAK_LINES) > 0;

		// int len43 = len * 4 / 3;
		// byte[] outBuff = new byte[ ( len43 ) // Main 4:3
		// + ( (len % 3) > 0 ? 4 : 0 ) // Account for padding
		// + (breakLines ? ( len43 / MAX_LINE_LENGTH ) : 0) ]; // New lines
		// Try to determine more precisely how big the array needs to be.
		// If we get it right, we don't have to do an array copy, and
		// we save a bunch of memory.

		// Bytes needed for actual encoding
		int encLen = (len / 3) * 4 + ((len % 3 > 0) ? 4 : 0);

		if (breakLines) {
			encLen += encLen / MAX_LINE_LENGTH; // Plus extra newline characters
		}
		byte[] outBuff = new byte[encLen];

		int d = 0;
		int e = 0;
		int len2 = len - 2;
		int lineLength = 0;
		for (; d < len2; d += 3, e += 4) {
			encode3to4(source, d + off, 3, outBuff, e, options);

			lineLength += 4;
			if (breakLines && lineLength >= MAX_LINE_LENGTH) {
				outBuff[e + 4] = NEW_LINE;
				e++;
				lineLength = 0;
			} // end if: end of line
		} // en dfor: each piece of array

		if (d < len) {
			encode3to4(source, d + off, len - d, outBuff, e, options);
			e += 4;
		} // end if: some padding needed

		// Only resize array if we didn't guess it right.
		if (e <= outBuff.length - 1) {
			byte[] finalOut = new byte[e];
			System.arraycopy(outBuff, 0, finalOut, 0, e);
			// System.err.println("Having to resize array from " + outBuff.length + " to "
			// + e );
			return finalOut;
		}
		else {
			// System.err.println("No need to resize array.");
			return outBuff;
		}
	}        
        
// Taken from: https://github.com/spring-projects/spring-security/blob/5.5.6/crypto/src/main/java/org/springframework/security/crypto/codec/Base64.java
// Origin: hoos/genesis: master
private static int decode4to3(final byte[] source, final int srcOffset, final byte[] destination,
                                  final int destOffset, final int options) {

        // Lots of error checking and exception throwing
        if (source == null) {
            throw new NullPointerException("Source array was null.");
        } // end if
        if (destination == null) {
            throw new NullPointerException("Destination array was null.");
        } // end if
        if (srcOffset < 0 || srcOffset + 3 >= source.length) {
            throw new IllegalArgumentException(
                    String.format("Source array with length %d cannot have offset of %d and still process four bytes.",
                            source.length, srcOffset));
        } // end if
        if (destOffset < 0 || destOffset + 2 >= destination.length) {
            throw new IllegalArgumentException(String.format(
                    "Destination array with length %d cannot have offset of %d and still store three bytes.",
                    destination.length, destOffset));
        } // end if

        byte[] DECODABET = getDecodabet(options);

        // Example: Dk==
        if (source[srcOffset + 2] == EQUALS_SIGN) {
            // Two ways to do the same thing. Don't know which way I like best.
            // int outBuff = ( ( DECODABET[ source[ srcOffset ] ] << 24 ) >>> 6 )
            // | ( ( DECODABET[ source[ srcOffset + 1] ] << 24 ) >>> 12 );
            int outBuff = ((DECODABET[source[srcOffset]] & 0xFF) << 18)
                    | ((DECODABET[source[srcOffset + 1]] & 0xFF) << 12);

            destination[destOffset] = (byte) (outBuff >>> 16);
            return 1;
        }

        // Example: DkL=
        else if (source[srcOffset + 3] == EQUALS_SIGN) {
            // Two ways to do the same thing. Don't know which way I like best.
            // int outBuff = ( ( DECODABET[ source[ srcOffset ] ] << 24 ) >>> 6 )
            // | ( ( DECODABET[ source[ srcOffset + 1 ] ] << 24 ) >>> 12 )
            // | ( ( DECODABET[ source[ srcOffset + 2 ] ] << 24 ) >>> 18 );
            int outBuff = ((DECODABET[source[srcOffset]] & 0xFF) << 18)
                    | ((DECODABET[source[srcOffset + 1]] & 0xFF) << 12)
                    | ((DECODABET[source[srcOffset + 2]] & 0xFF) << 6);

            destination[destOffset] = (byte) (outBuff >>> 16);
            destination[destOffset + 1] = (byte) (outBuff >>> 8);
            return 2;
        }

        // Example: DkLE
        else {
            // Two ways to do the same thing. Don't know which way I like best.
            // int outBuff = ( ( DECODABET[ source[ srcOffset ] ] << 24 ) >>> 6 )
            // | ( ( DECODABET[ source[ srcOffset + 1 ] ] << 24 ) >>> 12 )
            // | ( ( DECODABET[ source[ srcOffset + 2 ] ] << 24 ) >>> 18 )
            // | ( ( DECODABET[ source[ srcOffset + 3 ] ] << 24 ) >>> 24 );
            int outBuff = ((DECODABET[source[srcOffset]] & 0xFF) << 18)
                    | ((DECODABET[source[srcOffset + 1]] & 0xFF) << 12)
                    | ((DECODABET[source[srcOffset + 2]] & 0xFF) << 6) | ((DECODABET[source[srcOffset + 3]] & 0xFF));

            destination[destOffset] = (byte) (outBuff >> 16);
            destination[destOffset + 1] = (byte) (outBuff >> 8);
            destination[destOffset + 2] = (byte) (outBuff);

            return 3;
        }
    }

    //https://github.com/spring-projects/spring-boot-data-geode/blob/1.6.0/spring-geode/src/main/java/org/springframework/geode/core/util/ObjectUtils.java
    public static <T> T invoke(Object obj, String methodName) {

        return (T) Optional.ofNullable(obj)
                .map(Object::getClass)
                .map(type -> ReflectionUtils.findMethod(type, methodName))
                .map(ObjectUtils::makeAccessible)
                .map(method -> ReflectionUtils.invokeMethod(method, obj))
                .orElseThrow(() -> newIllegalArgumentException("Method [%1$s] on Object of type [%2$s] not found",
                        methodName, org.springframework.util.ObjectUtils.nullSafeClassName(obj)));
    }


    private void readWebSocketFrame() throws IOException {
        try {
            Frame frame = Frame.read(this.inputStream);
            if (frame.getType() == Frame.Type.PING) {
                writeWebSocketFrame(new Frame(Frame.Type.PONG));
            }
            else if (frame.getType() == Frame.Type.CLOSE) {
                throw new ConnectionClosedException();
            }
            else if (frame.getType() == Frame.Type.TEXT) {
                logger.debug(LogMessage.format("Received LiveReload text frame %s", frame));
            }
            else {
                throw new IOException("Unexpected Frame Type " + frame.getType());
            }
        }
        catch (SocketTimeoutException ex) {
            writeWebSocketFrame(new Frame(Frame.Type.PING));
            Frame frame = Frame.read(this.inputStream);
            if (frame.getType() != Frame.Type.PONG) {
                throw new IllegalStateException("No Pong");
            }
        }
    }
    /**
     * Trigger livereload for the client using this connection.
     * @throws IOException in case of I/O errors
     */
    void triggerReload() throws IOException {
        if (this.webSocket) {
            logger.debug("Triggering LiveReload");
            writeWebSocketFrame(new Frame("{\"command\":\"reload\",\"path\":\"/\"}"));
        }
    }

}

