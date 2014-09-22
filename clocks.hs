import Control.Monad (guard)



let params =  sortBy (compare `on` (\(fxtal, pllpre, plldiv, pllpost, fosc, 
            brgh, uxbrg, baud, bauderror, 
            dacfdiv, apostsclr, samplefreq) -> (fosc, 400-bauderror))) $ do
    let kHz = 1000
    let mHz = 1000000
    let samplefreq = 24 * kHz :: Float
    let dacclock = samplefreq * 256
    guard $ dacclock <=  25.6 * mHz 
    dacfdiv <- [1..128] 
    let aclk = dacclock * dacfdiv
    apostsclr <- [1,2,4,8,16,32,64,256] 
    let fosc = aclk * apostsclr
    guard $ fosc >= 12.5 * mHz 
    guard $ fosc <= 80 * mHz 
    let fcy = fosc / 2
    guard $ fcy > 36 * mHz 
    let fp = fosc / 2
    pllpost <- [2,4,8]
    let fvco = fosc * pllpost
    guard $ fvco >= 100 * mHz 
    guard $ fvco <= 200 * mHz 
    plldiv <- [2..513] 
    let fxtaldiv = fvco / plldiv
    guard $ fxtaldiv >= 0.8 * mHz 
    guard $ fxtaldiv <= 8.0 * mHz 
    pllpre <- [2..33] 
    let fxtal = fxtaldiv * pllpre
    guard $ fxtal <= 30 * mHz -- suggested lower bound for XT OSC
    guard $ fxtal >=  3.5 * mHz -- suggested upper bound for XT OSC
    guard $ fxtal `elem` [7.3728 * mHz, 8 * mHz, 8.192 * mHz, 14.7456 * mHz] -- only include common crystals
    brgh <- [0,1]
    let brghdiv = if brgh == 0 then 16 else 4
    let desired_baud = 31.25 * kHz
    let uxbrg_desired = (fp / (brghdiv * desired_baud)) - 1
    uxbrg <- [ fromIntegral (floor uxbrg_desired), 
               fromIntegral (ceiling uxbrg_desired) ] 
    guard $ uxbrg >= 0
    guard $ uxbrg <= 65535
    let baud = fp / ((brghdiv * uxbrg) + 1)
    let bauderror = abs (desired_baud - baud)
    guard $ bauderror <= (desired_baud/100.0)

    spipostscale1 <- [1,4,16,64]
    spipostscale2 <- [1..8]
    sck1 = fcy / spipostscale1 / spipostscale2
    guard $ sck1 < 25 * mHz
    return (fxtal, pllpre, plldiv, pllpost, fosc, 
            brgh, uxbrg, baud, bauderror, 
            dacfdiv, apostsclr, samplefreq)


