#' subscripts_for_index_rmajor 
#'
#' Compute the subscripts corresponding to a given 
#' linear index for an array in last-fastest 
#' ("row-major") order.
#' 
#' @param D Array of dimensions
#' @param ind Index from beginning of linear array
#' @return Array of subscripts
#' @export
subscripts_for_index_rmajor <- function(D, ind)  {
  m <- length(D)
  X <- rep(-1, m)
  off <- ind-1

  for (j in 0:(m-1))  {
    if (j == 0)  {
      s <- off %/% prod(D[2:m])
    } else if (j == (m-1))  {
      s <- off %% D[m]
    } else  {
      s <- (off %/% prod(D[(j+2):m])) %% D[j+1]
    }
    X[j+1] <- s
  }
  X <- X+1
  return(X)
}

#' subscripts_for_index_cmajor 
#'
#' Compute the subscripts corresponding to a given 
#' linear index for an array in first-fastest 
#' ("column-major") order.
#' 
#' @param D Array of dimensions
#' @param ind Index from beginning of linear array
#' @return Array of subscripts
#' @export
subscripts_for_index_cmajor <- function(D, ind)  {
  m <- length(D)
  X <- rep(-1, m)
  off <- ind-1

  for (j in 0:(m-1))  {
    if (j == 0)  {
      s <- off %% D[1]      
    } else if (j == m-1)  {
      s <- off %/% prod(D[1:m-1])
    } else  {
      s <- (off %/% prod(D[1:j])) %% D[j+1]
    } 
    X[j+1] <- s
  }
  X <- X+1
  return(X)
}

#' index_for_subscripts_rmajor
#'
#' Compute the index corresponding to a given
#' ordered set of subscripts for an array in
#' last-fastest ("row-major") order.
#'
#' @param D Array of dimensions
#' @param S Array of subscripts
#' @return linear index of element
#' @export
index_for_subscripts_rmajor <- function(D, S)  {
  m <- length(D)
  S <- S-1
  off <- 0
  for (j in 0:(m-1))  {
    p <- ifelse(j == m-1, 1, prod(D[(j+2):m]))
    off <- off + S[j+1]*p
  }
  return(off+1)
}

#' index_for_subscripts_cmajor
#'
#' Compute the index corresponding to a given
#' ordered set of subscripts for an array in
#' first-fastest ("column-major") order.
#'
#' @param D Array of dimensions
#' @param S Array of subscripts
#' @return linear index of element
#' @export
index_for_subscripts_cmajor <- function(D, S)  {
  m <- length(D)
  S <- S-1
  off <- 0
  for (j in 0:(m-1))  {
    p <- ifelse(j == 0, 1, prod(D[1:j]))
    off <- off + S[j+1]*p
  }
  return(off+1)
}
