uniform vec2 LensCenterL;
uniform vec2 ScreenCenterL;
uniform vec2 LensCenterR;
uniform vec2 ScreenCenterR;
uniform vec2 Scale;
uniform vec2 ScaleIn;
uniform vec4 HmdWarpParam;

uniform int riftMode;

varying vec2 u_texCoord;
uniform sampler2D texture;

vec2 HmdWarp(vec2 value)
{
    if (u_texCoord.x>0.5) {
        vec2  theta = (value - LensCenterR) * ScaleIn; // Scales to [-1, 1]
        float rSq = theta.x * theta.x + theta.y * theta.y;
        vec2  theta_one = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +
                                   HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
        return LensCenterR + Scale * theta_one;
    }else{
        vec2  theta = (value - LensCenterL) * ScaleIn; // Scales to [-1, 1]
        float rSq = theta.x * theta.x + theta.y * theta.y;
        vec2  theta_one = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +
                                   HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
        return LensCenterL + Scale * theta_one;
    }
    
}

void main()
{
    if (riftMode == 1) {
        if (u_texCoord.x>0.5){
            vec2 tc = HmdWarp(u_texCoord);
            if (!all(equal(clamp(tc, ScreenCenterR-vec2(0.25,0.5), ScreenCenterR+vec2(0.25,0.5)), tc)))
                gl_FragColor = vec4(0);
            else
                gl_FragColor = texture2D(texture, tc);
        }
        else{
            vec2 tc = HmdWarp(u_texCoord);
            if (!all(equal(clamp(tc, ScreenCenterL-vec2(0.25,0.5), ScreenCenterL+vec2(0.25,0.5)), tc)))
                gl_FragColor = vec4(0);
            else
                gl_FragColor = texture2D(texture, tc);
        }
    }else{
        gl_FragColor = texture2D(texture, u_texCoord);
    }
}